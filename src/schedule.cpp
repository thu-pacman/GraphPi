#include "../include/schedule.h"
#include <cstdio>
#include <cstring>
#include <assert.h>
#include <algorithm>

Schedule::Schedule(const Pattern& pattern, bool &is_valid, bool use_performance_modeling, std::vector<long long> &graph_degree_info, std::vector<long long> &graph_size_info)
{
    is_valid = true;
    size = pattern.get_size();
    adj_mat = new int[size * size];

    if( use_performance_modeling) {

        const int* pattern_adj_mat = pattern.get_adj_mat_ptr();
        int *best_order;
        best_order = new int[size];

        performance_modeling(pattern_adj_mat, best_order, graph_degree_info, graph_size_info);
        int *rank;
        rank = new int[size];
        for(int i = 0; i < size; ++i) rank[best_order[i]] = i;

        for(int i = 0; i < size; ++i)
            for(int j = 0; j < size; ++j)
                adj_mat[INDEX(rank[i], rank[j], size)] = pattern_adj_mat[INDEX(i, j, size)]; 
        delete[] rank;
        delete[] best_order;
    }
    else {
        memcpy(adj_mat, pattern.get_adj_mat_ptr(), size * size * sizeof(int));
    }

    // The I-th loop consists of at most the intersection of i-1 VertexSet.
    // So the max number of prefix = 0 + 1 + ... + size-1 = size * (size-1) / 2
    int max_prefix_num = size * (size - 1) / 2;
    father_prefix_id = new int[max_prefix_num];
    last = new int[size];
    next = new int[max_prefix_num];
    loop_set_prefix_id = new int[size];
    prefix = new Prefix[max_prefix_num];
    restrict_last = new int[size];
    restrict_next = new int[max_prefix_num];
    restrict_index = new int[max_prefix_num];
    memset(father_prefix_id, -1, max_prefix_num * sizeof(int));
    memset(last, -1, size * sizeof(int));
    memset(next, -1, max_prefix_num * sizeof(int));
    memset(restrict_last, -1, size * sizeof(int));
    memset(restrict_next, -1, max_prefix_num * sizeof(int));

    total_prefix_num = 0;
    total_restrict_num = 0;

    // The I-th vertex must connect with at least one vertex from 0 to i-1.
    for (int i = 1; i < size; ++i)
    {
        bool valid = false;
        for (int j = 0; j < i; ++j)
            if (adj_mat[INDEX(i, j, size)])
            {
                valid = true;
                break;
            }
        if (valid == false)
        {
//            printf("Invalid Schedule!\n");
            is_valid = false;
            return;
        }
    }

    build_loop_invariant();


}

Schedule::~Schedule()
{
    delete[] adj_mat;
    delete[] father_prefix_id;
    delete[] last;
    delete[] next;
    delete[] loop_set_prefix_id;
    delete[] prefix;
    delete[] restrict_last;
    delete[] restrict_next;
    delete[] restrict_index;
}

void Schedule::build_loop_invariant()
{
    int* tmp_data = new int[size];
    loop_set_prefix_id[0] = -1;
    for (int i = 1; i < size; ++i)
    {
        int data_size = 0;
        for (int j = 0; j < i; ++j)
            if (adj_mat[INDEX(i, j, size)])
                tmp_data[data_size++] = j;
        loop_set_prefix_id[i] = find_father_prefix(data_size, tmp_data);
    }
    assert(total_prefix_num <= size * (size - 1) / 2);
    delete[] tmp_data;
}

int Schedule::find_father_prefix(int data_size, const int* data)
{
    if (data_size == 0)
        return -1;
    int num = data[data_size - 1];
    for (int prefix_id = last[num]; prefix_id != -1; prefix_id = next[prefix_id])
        if (prefix[prefix_id].equal(data_size, data))
            return prefix_id;
    
    // not found, create new prefix and find its father prefix id recursively
    int father = find_father_prefix(data_size - 1, data);
    father_prefix_id[total_prefix_num] = father;
    next[total_prefix_num] = last[num];
    last[num] = total_prefix_num;
    prefix[total_prefix_num].init(data_size, data);
    ++total_prefix_num;
    return total_prefix_num - 1;
}

void Schedule::add_restrict(const std::vector< std::pair<int, int> >& restricts)
{
    int max_prefix_num = size * (size - 1) / 2;
    memset(restrict_last, -1, size * sizeof(int));
    memset(restrict_next, -1, max_prefix_num * sizeof(int));
    total_restrict_num = 0;
    for (const auto& p : restricts)
    {
        // p.first must be greater than p.second
        restrict_index[total_restrict_num] = p.first;
        restrict_next[total_restrict_num] = restrict_last[p.second];
        restrict_last[p.second] = total_restrict_num;
        ++total_restrict_num;
    }
}

// return the number of isomorphism
int Schedule::aggresive_optimize(const int *adj_mat, std::vector< std::pair<int, int> >& ordered_pairs) const
{
    std::vector< std::vector<int> > isomorphism_vec = get_isomorphism_vec(adj_mat);
    int ret = isomorphism_vec.size();

    std::vector< std::vector< std::vector<int> > > permutation_groups;
    permutation_groups.clear();
    for (const std::vector<int>& v : isomorphism_vec)
        permutation_groups.push_back(calc_permutation_group(v, size));

    ordered_pairs.clear();

    // delete permutation group which contains 1 permutation with 2 elements and some permutation with 1 elements,
    // and record the corresponding restriction.
    for (unsigned int i = 0; i < permutation_groups.size(); )
    {
        int two_element_number = 0;
        std::pair<int, int> found_pair;
        for (const std::vector<int>& v : permutation_groups[i])
            if (v.size() == 2)
            {
                ++two_element_number;
                found_pair = std::pair<int ,int>(v[0], v[1]);
            }
            else if (v.size() != 1)
            {
                two_element_number = -1;
                break;
            }
        if (two_element_number == 1)
        {
            permutation_groups.erase(permutation_groups.begin() + i);
            isomorphism_vec.erase(isomorphism_vec.begin() + i);
            ordered_pairs.push_back(found_pair);
            assert(found_pair.first < found_pair.second);
        }
        else
            ++i;
    }

    Pattern base_dag(size);
    for (const std::pair<int, int>& pair : ordered_pairs)
        base_dag.add_ordered_edge(pair.first, pair.second);

    bool changed = true;
    while (changed && isomorphism_vec.size() != 1)
    {
        // use restrictions to delete other isomophism
        for (unsigned int i = 0; i < isomorphism_vec.size(); )
        {
            Pattern test_dag(base_dag);
            const std::vector<int>& iso = isomorphism_vec[i];
            for (const std::pair<int, int>& pair : ordered_pairs)
                test_dag.add_ordered_edge(iso[pair.first], iso[pair.second]);
            if (test_dag.is_dag() == false) // is not dag means conflict
            {
                permutation_groups.erase(permutation_groups.begin() + i);
                isomorphism_vec.erase(isomorphism_vec.begin() + i);
            }
            else
                ++i;
        }

        changed = false;
        std::pair<int, int> found_pair;
        for (unsigned int i = 0; i < permutation_groups.size(); )
        {
            int two_element_number = 0;
            for (const std::vector<int>& v : permutation_groups[i])
                if (v.size() == 2)
                {
                    ++two_element_number;
                    found_pair = std::pair<int ,int>(v[0], v[1]);
                    break;
                }
            if (two_element_number >= 1)
            {
                permutation_groups.erase(permutation_groups.begin() + i);
                isomorphism_vec.erase(isomorphism_vec.begin() + i);
                assert(found_pair.first < found_pair.second);
                ordered_pairs.push_back(found_pair);
                base_dag.add_ordered_edge(found_pair.first, found_pair.second);
                changed = true;
                break;
            }
            else
                ++i;
        }
    }
    assert(isomorphism_vec.size() == 1);
    return ret;
}

std::vector< std::vector<int> > Schedule::get_isomorphism_vec(const int *adj_mat) const
{
    unsigned int pow = 1;
    for (int i = 2; i <= size; ++i)
        pow *= i;
    std::vector< std::vector<int> > vec;
    vec.clear();
    bool use[size];
    for (int i = 0; i < size; ++i)
        use[i] = false;
    std::vector<int> tmp_vec;
    get_full_permutation(vec, use, tmp_vec, 0);
    assert(vec.size() == pow);
    std::vector< std::vector<int> > isomorphism_vec;
    isomorphism_vec.clear();
    for (const std::vector<int>& v : vec)
    {
        bool flag = true;
        for (int i = 0; i < size; ++i)
            for (int j = i + 1; j < size; ++j)
                if (adj_mat[INDEX(i, j, size)] != 0)
                    if (adj_mat[INDEX(v[i], v[j], size)] == 0) // not isomorphism
                    {
                        flag = false;
                        break;
                    }
        if (flag == true)
            isomorphism_vec.push_back(v);
    }
    return isomorphism_vec;
}

void Schedule::get_full_permutation(std::vector< std::vector<int> >& vec, bool use[], std::vector<int> tmp_vec, int depth) const
{
    if (depth == size)
    {
        vec.push_back(tmp_vec);
        return;
    }
    for (int i = 0; i < size; ++i)
        if (use[i] == false)
        {
            use[i] = true;
            tmp_vec.push_back(i);
            get_full_permutation(vec, use, tmp_vec, depth + 1);
            tmp_vec.pop_back();
            use[i] = false;
        }
}

std::vector< std::vector<int> > Schedule::calc_permutation_group(const std::vector<int> vec, int size)
{
    bool use[size];
    for (int i = 0; i < size; ++i)
        use[i] = false;
    std::vector< std::vector<int> > res;
    res.clear();
    for (unsigned int i = 0; i < vec.size(); ++i)
        if (use[i] == false)
        {
            std::vector<int> tmp_vec;
            tmp_vec.clear();
            tmp_vec.push_back(i);
            use[i] = true;
            int x = vec[i];
            while (use[x] == false)
            {
                use[x] = true;
                tmp_vec.push_back(x);
                x = vec[x];
            }
            res.push_back(tmp_vec);
        }
    return res;
}

void Schedule::performance_modeling(const int* adj_mat, int* best_order, std::vector<long long> &graph_degree_info, std::vector<long long> &graph_size_info) {
    printf("begin performance_modeling\n");
//    double p = 1e-2;
//    double np = dataset_edge_size * 2.0 / dataset_vertex_size;
//    printf("p = %.6lf np = %.6lf\n", p, np);
    // p = 2.0 * dataset_edge_size / dataset_vertex_size / dataset_vertex_size;
    int magic_number = 0;
    int* order;
    int* rank;

    double* p_size;
    int max_degree = graph_degree_info.size();
    p_size = new double[max_degree];
    
    p_size[0] = graph_degree_info[0];
    for(int i = 1;i < max_degree; ++i) {
        long long tmp = 1;
        long long n = graph_degree_info[0] / 100000;
        for(long long j = n; j > n - i; --j)
            tmp *= j;
        for(long long j = 2; j <= i; ++j)
            tmp /= j;
        p_size[i] = graph_degree_info[i] * 1.0 / graph_size_info[i];
        if( i == 2) {
            puts("in");
       //     p_size[i] = graph_degree_info[i] * 1.0 / (7515023 * 3);
            p_size[i] = 7515023.0 / 16518948 / 2; 
        }
        printf("p_size[%d] = %.6lf\n", i, p_size[i]);
    }

//    double* pp;

    order = new int[size];
    rank = new int[size];
//    pp = new double[size]; 

 //   pp[0] = 1;
 //   for(int i = 1; i < size; ++i) pp[i] = pp[i - 1] * p;
    for(int i = 0; i < size; ++i) order[i] = i;
    double min_val;
    bool have_best = false;
    std::vector<int> invariant_size[size];
    do {
        // check if is valid schedule
        bool is_valid = true;
        for(int i = 1; i < size; ++i) {
            bool have_edge = false;
            for(int j = 0; j < i; ++j)
                if( adj_mat[INDEX(order[i], order[j], size)]) {
                    have_edge = true;
                    break;
                }
            if( have_edge == false) {
                is_valid = false;
                break;
            }
        }
        if( is_valid == false ) continue;
        
        for(int i = 0; i < size; ++i) rank[order[i]] = i;
        int* cur_adj_mat;
        cur_adj_mat = new int[size*size];
        for(int i = 0; i < size; ++i)
            for(int j = 0; j < size; ++j)
                cur_adj_mat[INDEX(rank[i], rank[j], size)] = adj_mat[INDEX(i, j, size)];

        std::vector< std::pair<int,int> > restricts;
        int multiplicity = aggresive_optimize(cur_adj_mat, restricts);
        int restricts_size = restricts.size();
        std::sort(restricts.begin(), restricts.end());
        double* sum;
        sum = new double[restricts_size];
        for(int i = 0; i < restricts_size; ++i) sum[i] = 0;
        int* tmp;
        tmp = new int[size];
        for(int i = 0; i < size; ++i) tmp[i] = i;
        do {
            for(int i = 0; i < restricts_size; ++i)
                if(tmp[restricts[i].first] > tmp[restricts[i].second]) {
                    sum[i] += 1;
                }
                else break;
        } while( std::next_permutation(tmp, tmp + size));
        double total = 1;
        for(int i = 2; i <= size; ++i) total *= i;
        for(int i = 0; i < restricts_size; ++i)
            sum[i] = sum[i] /total;
        for(int i = restricts_size - 1; i > 0; --i)
            sum[i] /= sum[i - 1];

        double val = 1;
        for(int i = 0; i < size; ++i) invariant_size[i].clear();
        for(int i = size - 1; i >= 0; --i) {
            int cnt_forward = 0;
            int cnt_backward = 0;
            for(int j = 0; j < i; ++j)
                if(cur_adj_mat[INDEX(j, i, size)])
                    ++cnt_forward;
            for(int j = i + 1; j < size; ++j)
                if(cur_adj_mat[INDEX(j, i, size)])
                    ++cnt_backward;

            int c = cnt_forward;
            for(int j = i - 1; j >= 0; --j)
                if(cur_adj_mat[INDEX(j, i, size)])
                    invariant_size[j].push_back(c--);

            for(int j = 0; j < invariant_size[i].size(); ++j)
                if(invariant_size[i][j] > 1) 
                    val += p_size[invariant_size[i][j] - 1] + p_size[1];
//                    val += np * pp[invariant_size[i][j] - 2] + np;
            for(int j = 0; j < restricts_size; ++j)
                if(restricts[j].second == i)
                    val *=  sum[j];
            val *= p_size[cnt_forward];
//            if( i == 0) val *= dataset_vertex_size;
//            else val *= (magic_number + np * pp[cnt_forward - 1]);
        
        }
        if( have_best == false || val < min_val) {
            have_best = true;
            for(int i = 0; i < size; ++i)
                best_order[i] = order[i];
            min_val = val;
        }
        
        for(int i = 0; i < size; ++i)
            printf("%d ", order[i]);
        printf("%.6lf\n", val);

        delete[] cur_adj_mat;
        delete[] sum;
        delete[] tmp;

    } while( std::next_permutation(order, order + size) );

    delete[] order;
    delete[] rank;
    delete[] p_size;
//    delete[] pp;
}
