#pragma once
#include "schedule.h"
class VertexSet
{
public:
    VertexSet();
    // allocate new memory according to max_intersection_size
    void init();
    // use memory from Graph, do not allocate new memory
    void init(int input_size, int* input_data);
    ~VertexSet();
    void intersection(const VertexSet& set0, const VertexSet& set1);
    //set1 is unordered
    static int unorderd_subtraction_size(const VertexSet& set0, const VertexSet& set1);
    void insert_ans_sort(int val);
    inline int get_size() const { return size;}
    inline int get_data(int i) const { return data[i];}
    inline const int* get_data_ptr() const { return data;}
    inline int* get_data_ptr() { return data;}
    inline void push_back(int val) { data[size++] = val;}
    inline void pop_back() { --size;}
    bool has_data(int val);
    static int max_intersection_size;
    void build_vertex_set(const Schedule& schedule, const VertexSet* vertex_set, int* input_data, int input_size, int prefix_id);
private:
    int* data;
    int size;
    bool allocate;
};