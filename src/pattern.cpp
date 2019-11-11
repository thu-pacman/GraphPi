#include "../include/pattern.h"
#include <assert.h>
#include <cstring>
#include <set>
#include <vector>
#include <cstdio>

Pattern::Pattern(int _size)
{
    size = _size;
    adj_mat = new int[size * size];
    memset(adj_mat, 0, size * size * sizeof(int));
}

Pattern::~Pattern()
{
    delete[] adj_mat;
}

Pattern::Pattern(const Pattern& p)
{
    size = p.get_size();
    adj_mat = new int[size * size];
    memcpy(adj_mat, p.get_adj_mat_ptr(), size * size * sizeof(int));
}

void Pattern::add_edge(int x, int y)
{
    adj_mat[INDEX(x, y, size)] = 1;
    adj_mat[INDEX(y, x, size)] = 1;
}

void Pattern::del_edge(int x, int y)
{
    adj_mat[INDEX(x, y, size)] = 0;
    adj_mat[INDEX(y, x, size)] = 0;
}

bool Pattern::check_connected() const
{
    bool vis[size];
    for (int i = 0; i < size; ++i)
        vis[i] = false;
    int que[size];
    int head = 0;
    int tail = 0;
    que[0] = 0;
    vis[0] = true;
    //bfs
    while (head <= tail)
    {
        int x = que[head++];
        for (int i = 0; i < size; ++i)
            if (adj_mat[INDEX(x, i, size)] != 0 && vis[i] == false)
            {
                vis[i] = true;
                que[++tail] = i;
            }
    }
    for (int i = 0; i < size; ++i)
        if (vis[i] == false)
            return false;
    return true;
}

void Pattern::get_full_permutation(std::vector< std::vector<int> >& vec, bool use[], std::vector<int> tmp_vec, int depth) const
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

void Pattern::count_all_isomorphism(std::set< std::set<int> >& s) const
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
    std::set<int> edge_set;
    for (const std::vector<int>& v : vec)
    {
        edge_set.clear();
        for (int i = 0; i < size; ++i)
            for (int j = i + 1; j < size; ++j)
                if (adj_mat[INDEX(i, j, size)] != 0)
                {
                    if (v[i] < v[j])
                        edge_set.insert(v[i] * size + v[j]);
                    else
                        edge_set.insert(v[j] * size + v[i]);
                }
                    
        if (s.count(edge_set) == 0)
            s.insert(edge_set);
    }
}

void Pattern::print() const
{
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (adj_mat[INDEX(i, j, size)] != 0)
                printf("(%d,%d) ", i, j);
    printf("\n");
}

bool Pattern::is_dag() const
{
    int degree[size];
    memset(degree, 0, size * sizeof(int));
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (adj_mat[INDEX(i, j, size)] > 0)
                ++degree[j];
    int que[size];
    int head = 0;
    int tail = -1;
    for (int i = 0; i < size; ++i)
        if (degree[i] == 0)
            que[++tail] = i;
    while (head <= tail)
    {
        int x = que[head++];
        for (int j = 0; j < size; ++j)
            if (adj_mat[INDEX(x, j, size)] > 0)
            {
                --degree[j];
                if (degree[j] == 0)
                    que[++tail] = j;
            }
    }
    if (tail == size - 1)
        return true;
    else
        return false;
}

std::vector< std::vector<int> > Pattern::calc_permutation_group(const std::vector<int> vec, int size)
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

std::vector< std::vector<int> > Pattern::get_isomorphism_vec() const
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

// return the number of isomorphism
int Pattern::aggresive_optimize(std::vector< std::pair<int, int> >& ordered_pairs) const
{
    std::vector< std::vector<int> > isomorphism_vec = get_isomorphism_vec();
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