#pragma once
#include <set>
#include <vector>

#ifndef INDEX
#define INDEX(x,y,n) ((x)*(n)+(y)) 
#endif

class Pattern
{
public:
    Pattern(int _size);
    ~Pattern();
    Pattern(const Pattern& p);
    void add_edge(int x, int y);
    void del_edge(int x, int y);
    inline int get_size() const {return size;}
    inline const int* get_adj_mat_ptr() const {return adj_mat;}
    bool check_connected();
    void count_all_isomorphism(std::set< std::set<int> >& s);
    void print() const;
private:
    Pattern& operator =(const Pattern&);
    void get_full_permutation(std::vector< std::vector<int> >& vec, bool use[], std::vector<int> tmp_vec, int depth);
    int* adj_mat;
    int size;
};