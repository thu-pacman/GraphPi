#include "../include/graphmpi.h"
#include "../include/common.h"
#include <mpi.h>
#include <cstring>
#include <omp.h>
#include <cstdio>

Graphmpi& Graphmpi::getinstance() {
    static Graphmpi gm;
    return gm;
}

void Graphmpi::init(int _threadcnt, Graph* _graph, int schedule_size) {
    threadcnt = _threadcnt;
    graph = _graph;
    int provided;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    starttime = get_wall_time();
    idlethreadcnt = 0;
    for (int i = 0; i < threadcnt; i++) {
        data[i] = new int[MESSAGE_SIZE];
        memset(data[i], -1, sizeof(int) * MESSAGE_SIZE);
        lock[i].test_and_set();
    }
    qlock.clear();
    chunksize = CHUNK_CONST * (MAXTHREAD - 1) * comm_sz;
    if (schedule_size > 3) chunksize /= graph->e_cnt / graph->v_cnt * (schedule_size - 3);
    chunksize = std::max(chunksize, 1);
    printf("chunksize = %d\n", chunksize);
    fflush(stdout);
}

long long Graphmpi::runmajor() {
    long long tot_ans = 0;
    const int IDLE = 2, END = 3, OVERWORK = 4, REPORT = 5, SERVER = 0, OVERWORKSIZE = 6;
    static int recv[MESSAGE_SIZE], send[MESSAGE_SIZE];
    MPI_Request sendrqst, recvrqst;
    MPI_Status status;
    MPI_Irecv(recv, sizeof(recv), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvrqst);
    int idlenodecnt = 0, *iter = new int[comm_sz], *edgel = new int[graph->v_cnt], *edger = graph->vertex + 1;
    memset(iter, 0, sizeof(int) * comm_sz);
    memcpy(edgel, graph->vertex, sizeof(int) * graph->v_cnt);
    auto get_send = [&](int last_work, int thread, int node) {
        send[0] = OVERWORK;
        send[4] = 0;
        send[5] = thread;
        if (~last_work && edgel[last_work] < edger[last_work]) {
            send[1] = last_work;
            send[2] = edgel[last_work];
            send[3] = edgel[last_work] = std::min(edgel[last_work] + chunksize, edger[last_work]);
        }
        else {
            int &i = iter[node];
            for (; i < graph->v_cnt && edgel[i] == edger[i]; i++);
            if (i == graph->v_cnt) send[1] = -1;
            else {
                send[1] = i;
                send[2] = edgel[i];
                send[3] = edgel[i] = std::min(edgel[i] + chunksize, edger[i]);
                i++;
            }
        }
    };
    for (;;) {
        int testflag = 0;
        MPI_Test(&recvrqst, &testflag, &status);
        if (testflag) {
            if (recv[0] == IDLE) {
                get_send(recv[1], recv[2], status.MPI_SOURCE);
                MPI_Isend(send, OVERWORKSIZE, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &sendrqst);
            }
            else if (recv[0] == OVERWORK) {
                memcpy(data[recv[5]], recv, OVERWORKSIZE * sizeof(recv[0]));
                lock[recv[5]].clear();
            }
            else if (recv[0] == REPORT) {
                tot_ans += (((long long)(recv[1]) << 32) | (unsigned)recv[2]);
                idlenodecnt++;
            }
            else if (recv[0] == END) {
                tot_ans = (((long long)(recv[1]) << 32) | (unsigned)recv[2]);
                break;
            }
            MPI_Irecv(recv, sizeof(recv), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvrqst);
        }
        bool tmpflag;
        int tmpthread;
        for (;qlock.test_and_set(););
        if (tmpflag = !idleq.empty()) {
            tmpthread = idleq.front();
            idleq.pop();
        }
        qlock.clear();
        if (tmpflag) {
            if (my_rank) {
                send[0] = IDLE;
                send[1] = data[tmpthread][1];
                send[2] = tmpthread;
                MPI_Isend(send, 3, MPI_INT, 0, SERVER, MPI_COMM_WORLD, &sendrqst);
            }
            else {
                get_send(data[tmpthread][1], tmpthread, SERVER);
                memcpy(data[tmpthread], send, OVERWORKSIZE * sizeof(send[0]));
                lock[tmpthread].clear();
            }
        }
        if (idlethreadcnt == threadcnt - 1) {
            idlethreadcnt = -1;
            if (my_rank) {
                send[0] = REPORT;
                send[1] = node_ans >> 32;
                send[2] = node_ans;
                MPI_Isend(send, 3, MPI_INT, 0, SERVER, MPI_COMM_WORLD, &sendrqst);
            }
            else {
                idlenodecnt++;
                tot_ans += node_ans;
            }
        }
        if (idlenodecnt == comm_sz) {
            for (int i = 1; i < comm_sz; i++) {
                //MPI_Wait(&sendrqst, &status);
                send[0] = END;
                send[1] = tot_ans >> 32;
                send[2] = tot_ans;
                MPI_Isend(send, 3, MPI_INT, i, SERVER, MPI_COMM_WORLD, &sendrqst);
            }
            break;
        }
    }
    delete[] iter;
    delete[] edgel;
    return tot_ans;
}

Graphmpi::Graphmpi() {}

Graphmpi::~Graphmpi() {
    for (int i = 0; i < threadcnt; i++) delete[] data[i];
    MPI_Finalize();
}

int* Graphmpi::get_edge_range() {
    int thread_num = omp_get_thread_num();
    for (;qlock.test_and_set(););
    idleq.push(thread_num);
    qlock.clear();
    for (;lock[thread_num].test_and_set(););
    return ~data[thread_num][1] ? data[thread_num] : nullptr;
}

void Graphmpi::report(long long local_ans) {
#pragma omp atomic
    node_ans += local_ans;
#pragma omp atomic
    idlethreadcnt++;
    printf("node = %d, thread = %d, local_ans = %lld, time = %f\n", my_rank, omp_get_thread_num(), local_ans, get_wall_time() - starttime);
    fflush(stdout);
}

void Graphmpi::set_loop(int *data, int size) {
    int k = omp_get_thread_num();
    loop_data[k] = data;
    loop_size[k] = size;
}

void Graphmpi::get_loop(int *&data, int &size) {
    if (loop_flag) {
        int k = omp_get_thread_num();
        data = loop_data[k];
        size = loop_size[k];
    }
}

void Bx2kQueue::push(int k) {
    a[t++] = k;
}

int Bx2kQueue::front() {
    return a[h];
}

bool Bx2kQueue::empty() {
    return h == t;
}

void Bx2kQueue::pop() {
    h++;
}
