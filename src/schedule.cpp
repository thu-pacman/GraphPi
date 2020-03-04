#include "../include/schedule.h"
#include <cstdio>
#include <cstring>
#include <assert.h>
#include <algorithm>

Schedule::Schedule(const Pattern& pattern, bool &is_pattern_valid, int performance_modeling_type, int v_cnt, int e_cnt)
{
    is_pattern_valid = true;
    size = pattern.get_size();
    adj_mat = new int[size * size];

    //Initialize adj_mat
    //If we use performance_modeling, we may change the order of vertex,
    //the best order produced by performance_modeling(...) is saved in best_order[]
    //Finally, we use best_order[] to relocate adj_mat
    if( performance_modeling_type != 0) {

        const int* pattern_adj_mat = pattern.get_adj_mat_ptr();
        int *best_order;
        best_order = new int[size];

        if( performance_modeling_type == 1) {
            performance_modeling(pattern_adj_mat, best_order, v_cnt, e_cnt);
        }
        else {
            GraphZero_performance_modeling(pattern_adj_mat, best_order, v_cnt, e_cnt);
        }
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
        // not use performance_modeling, simply copy the adj_mat from pattern
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
    in_exclusion_optimize_num = 0;
    
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
            //Invalid Schedule
            is_pattern_valid = false;
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

int Schedule::get_max_degree() {
    int mx = 0;
    for(int i = 0; i < size; ++i) {
        int cnt = 0;
        for(int j = 0; j < size; ++j)
            cnt += adj_mat[INDEX(i,j,size)];
        if(cnt > mx) mx = cnt;
    }
    return mx;
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
int Schedule::aggressive_optimize(const int *adj_mat, std::vector< std::pair<int, int> >& ordered_pairs) const
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

// return the number of isomorphism
// Schedule::aggressive_optimize(...) can only get one valid restrictions
// but in this func, we try our best to find more restrictions
// WARNING: the restrictions in ordered_pairs_vector may NOT CORRECT
int Schedule::aggressive_optimize_get_all_pairs(const int *adj_mat, std::vector< std::vector< std::pair<int, int> > >& ordered_pairs_vector) 
{
    std::vector< std::vector<int> > isomorphism_vec = get_isomorphism_vec(adj_mat);
    int ret = isomorphism_vec.size();

    std::vector< std::vector< std::vector<int> > > permutation_groups;
    permutation_groups.clear();
    for (const std::vector<int>& v : isomorphism_vec)
        permutation_groups.push_back(calc_permutation_group(v, size));

    ordered_pairs_vector.clear();

    std::vector< std::pair<int,int> > ordered_pairs;
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

    aggressive_optimize_dfs(base_dag, isomorphism_vec, permutation_groups, ordered_pairs, ordered_pairs_vector);

//    for(int i = 0; i < ordered_pairs_vector.size(); ++i)
//        std::sort(ordered_pairs_vector[i].begin(), ordered_pairs_vector[i].end());
//    std::unique(ordered_pairs_vector.begin(),ordered_pairs_vector.end());

    return ret;
}
void Schedule::aggressive_optimize_dfs(Pattern base_dag, std::vector< std::vector<int> > isomorphism_vec, std::vector< std::vector< std::vector<int> > > permutation_groups, std::vector< std::pair<int,int> > ordered_pairs, std::vector< std::vector< std::pair<int,int> > >& ordered_pairs_vector) {

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
    
    if(isomorphism_vec.size() == 1) {
        ordered_pairs_vector.push_back(ordered_pairs);
        return;
    }


    std::pair<int, int> found_pair;
    for (unsigned int i = 0; i < permutation_groups.size(); )
    {
        int two_element_number = 0;
        for (const std::vector<int>& v : permutation_groups[i])
            if (v.size() == 2)
            {
                ++two_element_number;
                found_pair = std::pair<int ,int>(v[0], v[1]);
                std::vector< std::vector< std::vector<int> > > next_permutation_groups = permutation_groups;
                std::vector< std::vector<int> > next_isomorphism_vec = isomorphism_vec;
                std::vector< std::pair<int,int> > next_ordered_pairs = ordered_pairs;
                Pattern next_base_dag = base_dag;
                
                next_permutation_groups.erase(next_permutation_groups.begin() + i);
                next_isomorphism_vec.erase(next_isomorphism_vec.begin() + i);
                assert(found_pair.first < found_pair.second);
                next_ordered_pairs.push_back(found_pair);
                next_base_dag.add_ordered_edge(found_pair.first, found_pair.second);
                
                aggressive_optimize_dfs(next_base_dag, next_isomorphism_vec, next_permutation_groups, next_ordered_pairs, ordered_pairs_vector);
            }
        if( two_element_number >= 1) {
            break;
        }
        else {
           ++i;
        }
    }

}

//-----GraphZero's algorithm for restriction generation
int Schedule::GraphZero_aggressive_optimize(const int *adj_mat, std::vector< std::pair<int, int> >& ordered_pairs) const { 
    std::vector< std::vector<int> > Aut;
    GraphZero_get_automorphisms(adj_mat, Aut);
    int multiplicity = Aut.size();

    std::vector< std::pair<int,int> > L;
    L.clear();

    for(int v = 0; v < size; ++v) { // iterate all elements in schedule
        std::vector< std::vector<int> > stabilized_aut;
        stabilized_aut.clear();

        for(int i = 0; i < Aut.size(); ++i) {
            std::vector<int>& x = Aut[i];
            if( x[v] == v) {
                stabilized_aut.push_back(x);
            }
            else {
                int x1  = v, x2 = x[v];
                if( x1 > x2) {
                    int tmp = x1;
                    x1 = x2;
                    x2 = tmp;
                }
                bool tag = true;
                std::pair<int,int> cur = std::make_pair(x1, x2);
                for(int j = 0; j < L.size(); ++j)
                    if( L[j]  == cur) {
                        tag = false;
                        break;
                    }
                if(tag) {
                    L.push_back(cur);
                }
            }
        }
        Aut = stabilized_aut;
    }
    
    ordered_pairs.clear(); // In GraphZero paper, this vector's name is 'L'

    for(int i = 0; i < L.size(); ++i) {
        bool tag = true;
        for(int j = 0; j < ordered_pairs.size(); ++j)
            if( L[i].second == ordered_pairs[j].second) {
                tag = false;
                if( L[i].first > ordered_pairs[j].first) ordered_pairs[j].first = L[i].first;
                break;
            }
        if(tag) ordered_pairs.push_back(L[i]);
    }
    return multiplicity;
}

void Schedule::GraphZero_get_automorphisms(const int *adj_mat, std::vector< std::vector<int> > &Aut) const {
    int p[size];
    Aut.clear();
    for(int i = 0; i < size; ++i) p[i] = i;
    do{
        bool tag = true;
        for(int i = 0; i < size; ++i) {
            for(int j = 0; j < size; ++j)
                if( adj_mat[INDEX(i, j, size)] != adj_mat[INDEX(p[i], p[j], size)]) {
                    tag = false;
                    break;
                }
            if( !tag ) break;
        }
        if(tag) {
            std::vector<int> tmp;
            tmp.clear();
            for(int i = 0; i < size; ++i) tmp.push_back(p[i]);
            Aut.push_back(tmp);
        }
    } while( std::next_permutation(p, p + size) );

}
//-----end GraphZero's function

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

void Schedule::performance_modeling(const int* adj_mat, int* best_order, int v_cnt, int e_cnt) {
    int* order;
    int* rank;

    double* p_size;
    int max_degree = get_max_degree();
    p_size = new double[max_degree];

    double p = e_cnt * 1.0 / v_cnt / v_cnt;
    
    p_size[0] = v_cnt;
    for(int i = 1;i < max_degree; ++i) {
        p_size[i] = p_size[i-1] * p;
    }

    order = new int[size];
    rank = new int[size];
    
    for(int i = 0; i < size; ++i) order[i] = i;
    double min_val;
    bool have_best = false;
    std::vector<int> invariant_size[size];
    do {
        // check whether it is valid schedule
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
        int multiplicity = GraphZero_aggressive_optimize(cur_adj_mat, restricts);
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
            for(int j = 0; j < restricts_size; ++j)
                if(restricts[j].second == i)
                    val *=  sum[j];
            val *= p_size[cnt_forward];
        
        }
        if( have_best == false || val < min_val) {
            have_best = true;
            for(int i = 0; i < size; ++i)
                best_order[i] = order[i];
            min_val = val;
        }
        
        delete[] cur_adj_mat;
        delete[] sum;
        delete[] tmp;

    } while( std::next_permutation(order, order + size) );

    delete[] order;
    delete[] rank;
    delete[] p_size;
}

void Schedule::init_in_exclusion_optimize(int optimize_num) {
    in_exclusion_optimize_num = optimize_num;

    int* id;
    id = new int[ optimize_num ];

    int* in_exclusion_val;
    in_exclusion_val = new int[ optimize_num * 2];

    for(int n = 1; n <= optimize_num; ++n) {
        DisjointSetUnion dsu(n);
        int m = n * (n - 1) / 2;
        
        in_exclusion_val[ 2 * n - 2 ] = 0;
        in_exclusion_val[ 2 * n - 1 ] = 0;
        
        if( n == 1) {
            ++in_exclusion_val[0];
            continue;
        }

        std::pair<int,int> edge[m];
        int e_cnt = 0;
        for(int i = 0; i < n; ++i)
            for(int j = 0; j < i; ++j)
                edge[e_cnt++] = std::make_pair(i,j);
        
        for(int s = 0; s < (1<<m); ++s) {
            dsu.init();
            int bit_cnt = 0;
            for(int i = 0; i < m; ++i) 
                if( s & (1<<i)) {
                    ++bit_cnt;
                    dsu.merge(edge[i].first, edge[i].second);
                }
            if( dsu.get_set_size() == 1) {
                if( bit_cnt & 1) ++in_exclusion_val[2 * n -1];
                else ++in_exclusion_val[ 2 * n - 2];
            }
        }
    }        

    in_exclusion_optimize_group.clear();
    in_exclusion_optimize_val.clear();

//    printf("begin get_in_exclusion_optimize_group(...)\n");
//    fflush(stdout);

    get_in_exclusion_optimize_group(0, id, 0, in_exclusion_val);

    delete[] id;
    delete[] in_exclusion_val;
}

void Schedule::get_in_exclusion_optimize_group(int depth, int* id, int id_cnt, int* in_exclusion_val) {
//    printf("depth %d id_cnt %d\n", depth, id_cnt);
//    fflush(stdout);
    if( depth == in_exclusion_optimize_num) {
        int* size = new int[id_cnt];
        for(int i = 0; i < id_cnt; ++i)
            size[i] = 0;
        for(int i = 0; i < in_exclusion_optimize_num; ++i)
            size[ id[i] ] ++;
        int val[2];
        val[0] = in_exclusion_val[ size[0] * 2 - 2 ];
        val[1] = in_exclusion_val[ size[0] * 2 - 1 ];
        for(int i = 1; i < id_cnt; ++i) {
            int tmp0 = val[0];
            int tmp1 = val[1];

            val[0] = tmp0 * in_exclusion_val[ size[i] * 2 - 2] + tmp1 * in_exclusion_val[ size[i] * 2 - 1];
            val[1] = tmp0 * in_exclusion_val[ size[i] * 2 - 1] + tmp1 * in_exclusion_val[ size[i] * 2 - 2];
        }

        std::vector< std::vector<int> > group;
        group.clear();
        for(int i = 0; i < id_cnt; ++i) {
            std::vector<int> cur;
            cur.clear();
            for(int j = 0; j < in_exclusion_optimize_num; ++j)
                if( id[j] == i) cur.push_back(j);
            group.push_back(cur);
        }

        in_exclusion_optimize_group.push_back(group);
        in_exclusion_optimize_val.push_back( val[0] - val[1] );

        delete[] size;
        return;
    }
    
    id[depth] = id_cnt;

    get_in_exclusion_optimize_group(depth + 1, id, id_cnt + 1, in_exclusion_val);
    
    for(int i = 0; i < id_cnt; ++i) {
        id[depth] = i;
        get_in_exclusion_optimize_group(depth + 1, id, id_cnt, in_exclusion_val);
    }
}

void Schedule::print_schedule() {
    printf("Schedule:\n");
    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j)
            printf("%d", adj_mat[INDEX(i,j,size)]);
        puts("");
    }
}

void Schedule::GraphZero_performance_modeling(const int *adj_mat, int* best_order, int v_cnt, int e_cnt) {
    int* order;
    int* rank;

    double* p_size;
    double* anti_p;
    int max_degree = get_max_degree();
    p_size = new double[max_degree];
    anti_p = new double[max_degree];

    double p = e_cnt * 1.0 / v_cnt / v_cnt;
    
    p_size[0] = v_cnt;
    for(int i = 1; i < max_degree; ++i) {
        p_size[i] = p_size[i-1] * p;
    }
    anti_p[0] = 1;
    for(int i = 1; i < max_degree; ++i) {
        anti_p[i] = anti_p[i-1] * (1-p);
    }

    order = new int[size];
    rank = new int[size];
    
    for(int i = 0; i < size; ++i) order[i] = i;
    double min_val;
    bool have_best = false;
    do {
        // check whether it is valid schedule
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
        int multiplicity = GraphZero_aggressive_optimize(cur_adj_mat, restricts);
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
        for(int i = size - 1; i >= 0; --i) {
            int cnt_forward = 0;
            int cnt_backward = 0;
            for(int j = 0; j < i; ++j)
                if(cur_adj_mat[INDEX(j, i, size)])
                    ++cnt_forward;

            for(int j = 0; j < restricts_size; ++j)
                if(restricts[j].second == i)
                    val *=  sum[j];
            val *= p_size[cnt_forward] * anti_p[i - cnt_forward];
        
        }
        if( have_best == false || val < min_val) {
            have_best = true;
            for(int i = 0; i < size; ++i)
                best_order[i] = order[i];
            min_val = val;
        }
        
        delete[] cur_adj_mat;
        delete[] sum;
        delete[] tmp;

    } while( std::next_permutation(order, order + size) );

    delete[] order;
    delete[] rank;
    delete[] p_size;
    delete[] anti_p;
}
