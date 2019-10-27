#include "../include/graph.h"
#include "../include/vertex_set.h"
#include "../include/common.h"
#include <cstdio>
#include <sys/time.h>
#include <unistd.h>
#include <cstdlib>
#include <omp.h>

int Graph::intersection_size(int v1,int v2) {
    int l1, r1;
    get_edge_index(v1, l1, r1);
    int l2, r2;
    get_edge_index(v2, l2, r2);
    int ans = 0;
    while(l1 < r1 && l2 < r2) {
        if(edge[l1] < edge[l2]) {
            ++l1;
        }
        else {
            if(edge[l2] < edge[l1]) {
                ++l2;
            }
            else {
                ++l1;
                ++l2;
                ++ans;
            }
        }
    }
    return ans;
}

int Graph::triangle_counting() {
    int ans = 0;
    for(int v = 0; v < v_cnt; ++v) {
        // for v in G
        int l, r;
        get_edge_index(v, l, r);
        for(int v1 = l; v1 < r; ++v1) {
            //for v1 in N(v)
            ans += intersection_size(v,edge[v1]);
        }
    }
    ans /= 6;
    return ans;
}

int Graph::triangle_counting_mt(int thread_count) {
    int ans = 0;
#pragma omp parallel num_threads(thread_count)
    {
        tc_mt(&ans);
    }
    ans /= 6;
    return ans;
}

void Graph::tc_mt(int *global_ans) {
    int my_ans = 0;
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();
    int left_v = (v_cnt / thread_count) * my_rank;
    int right_v = (v_cnt / thread_count) + left_v;
    if(my_rank == thread_count - 1) right_v = v_cnt;
    //TODO : use omp parallel for and try different schedule for load balance.
    for(int v = left_v; v < right_v; ++v) {
        // for v in G
        int l, r;
        get_edge_index(v, l, r);
        for(int v1 = l; v1 < r; ++v1) {
            //for v1 in N(v)
            my_ans += intersection_size(v,edge[v1]);
        }
    }
    #pragma omp critical
    {
        *global_ans += my_ans;
    }
}

void Graph::get_edge_index(int v, int& l, int& r) const
{
    l = vertex[v];
    r = vertex[v + 1];
}

void Graph::pattern_matching_func(const Schedule& schedule, VertexSet* vertex_set, VertexSet& subtraction_set, int& local_ans, int depth)
{
    int loop_set_prefix_id = schedule.get_loop_set_prefix_id(depth);
    int loop_size = vertex_set[loop_set_prefix_id].get_size();
    int* loop_data_ptr = vertex_set[loop_set_prefix_id].get_data_ptr();
    if (depth == schedule.get_size() - 1)
    {
        // TODO : try more kinds of calculation.
        // For example, we can maintain an ordered set, but it will cost more to maintain itself when entering or exiting recursion.
        if (loop_size > 0)
            local_ans += VertexSet::unorderd_subtraction_size(vertex_set[loop_set_prefix_id], subtraction_set);
        return;
    }
    for (int i = 0; i < loop_size; ++i)
    {
        int vertex = loop_data_ptr[i];
        if (subtraction_set.has_data(vertex))
            continue;
        int l, r;
        get_edge_index(vertex, l, r);
        for (int prefix_id = schedule.get_last(depth); prefix_id != -1; prefix_id = schedule.get_next(prefix_id))
        {
            vertex_set[prefix_id].build_vertex_set(schedule, vertex_set, &edge[l], r - l, prefix_id);
        }
        //subtraction_set.insert_ans_sort(vertex);
        subtraction_set.push_back(vertex);
        pattern_matching_func(schedule, vertex_set, subtraction_set, local_ans, depth + 1);
        subtraction_set.pop_back();
    }
}

int Graph::pattern_matching(const Schedule& schedule, int thread_count)
{
    int global_ans = 0;
    #pragma omp parallel num_threads(thread_count) reduction(+: global_ans)
    {
        VertexSet* vertex_set = new VertexSet[schedule.get_total_prefix_num()];
        VertexSet subtraction_set;
        subtraction_set.init();
        int local_ans = 0;
        // TODO : try different chunksize
        #pragma omp for schedule(dynamic)
        for (int vertex = 0; vertex < v_cnt; ++vertex)
        {
            int l, r;
            get_edge_index(vertex, l, r);
            for (int prefix_id = schedule.get_last(0); prefix_id != -1; prefix_id = schedule.get_next(prefix_id))
            {
                vertex_set[prefix_id].build_vertex_set(schedule, vertex_set, &edge[l], r - l, prefix_id);
            }
            //subtraction_set.insert_ans_sort(vertex);
            subtraction_set.push_back(vertex);
            pattern_matching_func(schedule, vertex_set, subtraction_set, local_ans, 1);
            subtraction_set.pop_back();
        }
        delete[] vertex_set;

        // TODO : Computing multiplicty for a pattern
        global_ans += local_ans;
    }
    return global_ans;
}