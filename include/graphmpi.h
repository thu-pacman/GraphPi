#pragma once
#include "graph.h"
#include <queue>
#include <atomic>

class Bx2k256Queue {
public:
    Bx2k256Queue();
    bool empty();
    void push(int);
    int front_and_pop();// can only be called by main thread

private:
    std::atomic_flag lock;
    unsigned char h, t;
    int q[256];
};

class Graphmpi {
public:
    static Graphmpi& getinstance();
    void init(int thread_count, Graph *graph, const Schedule& schedule); // get node range
    long long runmajor(); // mpi uses on major thread
    unsigned int* get_edge_range();
    void report(long long local_ans);
    void set_loop_flag();
    void set_loop(int*, int);
    void get_loop(int*&, int&);

private:
    static const int MAXTHREAD = 24, MESSAGE_SIZE = 5;
    Graph* graph;
    int *loop_data[MAXTHREAD], comm_sz, my_rank, idlethreadcnt, threadcnt, mpi_chunk_size, omp_chunk_size;
    unsigned int loop_size[MAXTHREAD], *data[MAXTHREAD];
    long long node_ans;
    double starttime;
    bool loop_flag = false, skip_flag; // loop_flag is set when using mpi; skip_flag is set when there is a restriction on the first pattern edge
    Bx2k256Queue idleq;
    //std::queue<int> idleq;
    std::atomic_flag lock[MAXTHREAD], qlock;
    bool initialized;
    Graphmpi();
    Graphmpi(const Graphmpi&&) = delete;
    Graphmpi(const Graphmpi&) = delete;
    Graphmpi& operator = (const Graphmpi&) = delete;
    ~Graphmpi();
};
