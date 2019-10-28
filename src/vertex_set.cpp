#include "../include/vertex_set.h"
#include <algorithm>

int VertexSet::max_intersection_size = -1;

VertexSet::VertexSet()
:data(nullptr), size(0), allocate(false)
{}

void VertexSet::init()
{
    if (allocate == true && data != nullptr)
        size = 0; // do not reallocate
    else
    {
        size = 0;
        allocate = true;
        data = new int[max_intersection_size];
    }
}

void VertexSet::init(int input_size, int* input_data)
{
    if (allocate == true && data != nullptr)
        delete[] data;
    size = input_size;
    data = input_data;
    allocate = false;
}

VertexSet::~VertexSet()
{
    if (allocate== true && data != nullptr)
        delete[] data;
}

void VertexSet::intersection(const VertexSet& set0, const VertexSet& set1)
{
    int i = 0;
    int j = 0;
    int size0 = set0.get_size();
    int size1 = set1.get_size();

    // TODO : Try more kinds of calculation.
    // Like
    // while (true)
    //     ..., if (++i == size0) break;
    //     ..., if (++j == size1) break;
    //     ......
    // Maybe we can also use binary search if one set is very small and another is large.
    while (i < size0 && j < size1)
    {
        int data0 = set0.get_data(i);
        int data1 = set1.get_data(j);
        if (data0 < data1)
            ++i;
        else if (data0 > data1)
            ++j;
        else
        {
            ++i;
            ++j;
            push_back(data0);
        }
    }
}

void VertexSet::build_vertex_set(const Schedule& schedule, const VertexSet* vertex_set, int* input_data, int input_size, int prefix_id)
{
    int father_id = schedule.get_father_prefix_id(prefix_id);
    if (father_id == -1)
        init(input_size, input_data);
    else
    {
        init();
        VertexSet tmp_vset;
        tmp_vset.init(input_size, input_data);
        intersection(vertex_set[father_id], tmp_vset);
    }
}

void VertexSet::insert_ans_sort(int val)
{
    int i;
    for (i = size - 1; i >= 0; --i)
        if (data[i] >= val)
            data[i + 1] = data[i];
        else
        {
            data[i + 1] = val;
            break;
        }
    if (i == -1)
        data[0] = val;
    ++size;
}

bool VertexSet::has_data(int val)
{
    for (int i = 0; i < size; ++i)
        if (data[i] == val)
            return true;
    return false;
}

int VertexSet::unorderd_subtraction_size(const VertexSet& set0, const VertexSet& set1)
{
    int size0 = set0.get_size();
    int size1 = set1.get_size();

    int ret = size0;
    const int* set0_ptr = set0.get_data_ptr();
    for (int j = 0; j < size1; ++j)
        if (std::binary_search(set0_ptr, set0_ptr + size0, set1.get_data(j)))
            --ret;
    return ret;
}