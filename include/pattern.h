#pragma once

#ifndef INDEX
#define INDEX(x,y,n) ((x)*(n)+(y)) 
#endif

class Pattern
{
public:
    Pattern(int _size);
    ~Pattern();
    void add_edge(int x, int y);
    inline int get_size() const {return size;}
    inline const int* get_adj_mat_ptr() const {return adj_mat;}
private:
    int* adj_mat;
    int size;
};