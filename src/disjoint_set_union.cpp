#include "../include/disjoint_set_union.h"

DisjointSetUnion::DisjointSetUnion(int n) {
    size = n;
    set_size = n;
    father = new int[n];
}

DisjointSetUnion::~DisjointSetUnion() {
    if( size > 0) {
        delete[] father;
    }
}

void DisjointSetUnion::init() {
    for(int i = 0; i < size; ++i)
        father[i] = i;
    set_size = size;
}

void DisjointSetUnion::merge(int id1, int id2) {
    int fa1 = get_father(id1);
    int fa2 = get_father(id2);
    if(fa1 != fa2) {
        --set_size;
        father[fa1] = fa2;
    }
}

int DisjointSetUnion::get_father(int a) {
    if( father[a] == a) return a;
    else return father[a] = get_father(father[a]);
}
