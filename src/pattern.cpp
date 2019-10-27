#include "../include/pattern.h"
#include <cstring>

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

void Pattern::add_edge(int x, int y)
{
    adj_mat[INDEX(x, y, size)] = 1;
    adj_mat[INDEX(y, x, size)] = 1;
}