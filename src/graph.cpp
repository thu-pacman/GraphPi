#include "../include/graph.h"
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
    double t1 = get_wall_time();
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
    double t2 = get_wall_time();
    printf("counting time: %.6lf\n", t2 - t1);
    return ans;
}

int Graph::triangle_counting_mt(int thread_count) {
    double t1 = get_wall_time();
    int ans = 0;
#pragma omp parallel num_threads(thread_count)
    {
        tc_mt(&ans);
    }
    ans /= 6;
    double t2 = get_wall_time();
    printf("counting time: %.6lf\n", t2 - t1);
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

double Graph::get_wall_time() {
    struct timeval time;
    if(gettimeofday(&time,NULL)) {
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * 0.000001;
}

void Graph::get_edge_index(int v, int& l, int& r) const
{
    l = vertex[v];
    r = vertex[v + 1];
}
