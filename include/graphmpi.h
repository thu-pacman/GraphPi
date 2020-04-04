#pragma once
#include "graph.h"
#include <queue>
#include <atomic>

class Graphmpi {
public:
    static Graphmpi& getinstance();
    void init(int thread_count, Graph *graph, int schedule_size); // get node range
    long long runmajor(); // mpi uses on major thread
    int* get_edge_range();
    void report(long long local_ans);
    void set_loop(int*, int);
    void get_loop(int*&, int&);
    void set_cur(int);
    bool loop_flag = false;

private:
    static const int MAXTHREAD = 24, CHUNK_CONST = 10, MESSAGE_SIZE = 16, ROLL_SIZE = 32768;
    Graph* graph;
    int comm_sz, my_rank, idlethreadcnt, threadcnt, chunksize, *data[MAXTHREAD], *loop_data[MAXTHREAD], loop_size[MAXTHREAD], min_cur;
    long long node_ans;
    double starttime;
    std::queue<int> idleq;
    //std::queue<int> idleq;
    std::atomic_flag lock[MAXTHREAD], qlock;
    Graphmpi();
    Graphmpi(const Graphmpi&&) = delete;
    Graphmpi(const Graphmpi&) = delete;
    Graphmpi& operator = (const Graphmpi&) = delete;
    ~Graphmpi();
};
