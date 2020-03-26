#pragma once
#include "graph.h"
#include <queue>
#include <atomic>

class Bx2kQueue {
public:
    void push(int);
    void pop();
    int front();
    bool empty();

private:
    static const int N = 256;
    int a[N];
    unsigned char h = 0, t = 0;
};

class Graphmpi {
public:
    static Graphmpi& getinstance();
    void init(int thread_count, Graph *graph, int schedule_size); // get node range
    long long runmajor(); // mpi uses on major thread
    int* get_edge_range();
    void report(long long local_ans);
    bool loop_flag = false;

private:
    static const int MAXTHREAD = 24, CHUNK_CONST = 10, MESSAGE_SIZE = 256;
    Graph* graph;
    int comm_sz, my_rank, idlethreadcnt, threadcnt, chunksize, *data[MAXTHREAD];
    long long node_ans;
    double starttime;
    Bx2kQueue idleq;
    //std::queue<int> idleq;
    std::atomic_flag lock[MAXTHREAD], qlock;
    Graphmpi();
    Graphmpi(const Graphmpi&&) = delete;
    Graphmpi(const Graphmpi&) = delete;
    Graphmpi& operator = (const Graphmpi&) = delete;
    ~Graphmpi();
};
