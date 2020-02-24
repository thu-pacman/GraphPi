#pragma once

class DisjointSetUnion {
public:
    DisjointSetUnion(int n);
    ~DisjointSetUnion();
    void init();
    void merge(int id1, int id2);
    inline int get_set_size() const { return set_size;}
    inline int get_size() const { return size;}

private:
    int get_father(int a);
    
    int size;
    int set_size;
    int* father;
};
