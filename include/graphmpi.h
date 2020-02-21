#pragma once
#include "graph.h"
#include <queue>
#include <atomic>

class Graphmpi {
public:
    static Graphmpi& getinstance();
    std::pair<int, int> init(int thread_count, Graph *graph); // get node range
    int runmajor(); // mpi uses on major thread
    int* getneighbor(int u); // return a int[] end with a -1
    int getdegree(); // this function can only be called immediately after calling getneighbor
    bool include(int u); // return whether u is in this process
    int get_startvertex();
    void report(long long local_ans);
    void end();

private:
    static const int MAXN = 1 << 22, MAXTHREAD = 24;
    Graph* graph;
    int comm_sz, my_rank, mynodel, mynoder, blocksize, idlethreadcnt, threadcnt, global_vertex, vertex[MAXTHREAD];
    long long node_ans = 0, tot_ans = 0;
    double starttime;
    std::queue<int> requestq, idleq;
    static int data[MAXTHREAD][MAXN], qrynode[MAXTHREAD], qrydest[MAXTHREAD], length[MAXTHREAD];
    std::atomic_flag lock[MAXTHREAD], global_vertex_lock;
    Graphmpi();
    Graphmpi(const Graphmpi&&) = delete;
    Graphmpi(const Graphmpi&) = delete;
    Graphmpi& operator = (const Graphmpi&) = delete;
    ~Graphmpi();
};
