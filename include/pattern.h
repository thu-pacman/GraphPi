#pragma once
#include <set>
#include <vector>
#include <cstdio>

#ifndef INDEX
#define INDEX(x,y,n) ((x)*(n)+(y)) 
#endif

enum PatternType {
    Rectangle,
    QG3,
    Pentagon,
    House,
    Hourglass,
    Cycle_6_Tri,
    Clique_7_Minus
};


class Pattern
{
public:
    Pattern(int _size, bool clique = false);
    Pattern(int _size, char* buffer);
    ~Pattern();
    Pattern(const Pattern& p);
    Pattern(PatternType type);
    void add_edge(int x, int y);
    void del_edge(int x, int y);
    inline void add_ordered_edge(int x, int y) { adj_mat[INDEX(x, y, size)] = 1;}
    inline int get_size() const {return size;}
    inline const int* get_adj_mat_ptr() const {return adj_mat;}
    bool check_connected() const;
    void count_all_isomorphism(std::set< std::set<int> >& s) const;
    void print() const;
    bool is_dag() const;
private:
    Pattern& operator =(const Pattern&);
    void get_full_permutation(std::vector< std::vector<int> >& vec, bool use[], std::vector<int> tmp_vec, int depth) const;
    int* adj_mat;
    int size;
};
