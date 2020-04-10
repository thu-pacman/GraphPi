#include "../include/pattern.h"
#include <assert.h>
#include <cstring>
#include <set>
#include <vector>
#include <cstdio>
#include <algorithm>

Pattern::Pattern(int _size, bool clique)
{
    size = _size;
    adj_mat = new int[size * size];
    memset(adj_mat, 0, size * size * sizeof(int));

    if( clique ) {
        for(int i = 0; i < size; ++i)
            for(int j = 0; j < i; ++j)
                add_edge(i,j);
    }
}

Pattern::Pattern(int _size, char *buffer) {
    size = _size;
    adj_mat = new int[size * size];
    memset(adj_mat, 0, size * size * sizeof(int));

    for(int i = 0; i < size; ++i)
        for(int j = 0; j < size; ++j)
            if(buffer[INDEX(i,j,size)] == '1')
                add_edge(i,j);
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

Pattern::Pattern(PatternType type) {
    if( type == PatternType::Rectangle) {
        size = 4;
        adj_mat = new int[size * size];
        memset(adj_mat, 0, size * size * sizeof(int));
        add_edge(0, 1);
        add_edge(0, 2);
        add_edge(1, 3);
        add_edge(2, 3);
        return;
    }
    if( type == PatternType::QG3) {
        size = 4;
        adj_mat = new int[size * size];
        memset(adj_mat, 0, size * size * sizeof(int));
        add_edge(0, 1);
        add_edge(0, 2);
        add_edge(0, 3);
        add_edge(1, 2);
        add_edge(1, 3);
        return;
    }
    if( type == PatternType::Pentagon) {
        size = 5;
        adj_mat = new int[size * size];
        memset(adj_mat, 0, size * size * sizeof(int));
        add_edge(0, 1); 
        add_edge(0, 2); 
        add_edge(1, 3); 
        add_edge(2, 4); 
        add_edge(3, 4); 
        return;
    }
    if( type == PatternType::House) {
        size = 5;
        adj_mat = new int[size * size];
        memset(adj_mat, 0, size * size * sizeof(int));
        add_edge(0, 1);
        add_edge(0, 2);
        add_edge(0, 3);
        add_edge(1, 3);
        add_edge(1, 4);
        add_edge(2, 4);
        return;
    }
    if( type == PatternType::Hourglass) {
        size = 6;
        adj_mat = new int[size * size];
        memset(adj_mat, 0, size * size * sizeof(int));
        add_edge(0, 1);
        add_edge(0, 2);
        add_edge(0, 4);
        add_edge(1, 2);
        add_edge(1, 5);
        add_edge(2, 3);
        add_edge(3, 4);
        add_edge(3, 5);
        add_edge(4, 5);
        return;
    }
    if( type == PatternType::Cycle_6_Tri) {
        size = 6;
        adj_mat = new int[size * size];
        memset(adj_mat, 0, size * size * sizeof(int));
        add_edge(0, 1);
        add_edge(0, 2);
        add_edge(1, 2);
        add_edge(1, 3);
        add_edge(1, 4);
        add_edge(2, 3);
        add_edge(2, 5);
        add_edge(0, 4);
        add_edge(0, 5);
        return;
    }
    if( type == PatternType::Clique_7_Minus) {
        size = 7;
        adj_mat = new int[size * size];
        memset(adj_mat, 0, size * size * sizeof(int));
        for(int i = 0; i < size; ++i)
            for(int j = 0; j < i; ++j)
                if( i != size - 1|| j != size - 2) 
                    add_edge(j, i);
        return;
    }

    printf("invalid pattern type!\n");
    assert(0);
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
