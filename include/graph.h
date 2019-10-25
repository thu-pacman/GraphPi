#pragma once
class Graph {
public:
    int v_cnt; // number of vertex
    int e_cnt; // number of edge
    
    int *edge; // edges
    int *vertex; // v_i's neighbor is in edge[ vertex[i], vertex[i+1]-1]
    

    Graph() {
        v_cnt = e_cnt = 0;
        edge = vertex = nullptr;
    }

    ~Graph() {
        if(edge != nullptr) delete[] edge;
        if(vertex != nullptr) delete[] vertex;
    }

    int intersection_size(int v1,int v2);

    //single thread triangle counting
    int triangle_counting();
    
    //multi thread triangle counting
    int triangle_counting_mt(int thread_count);
private:
    double get_wall_time();
    
    void tc_mt(int * global_ans);
};
