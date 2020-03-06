#include "../include/graphmpi.h"
#include "../include/common.h"
#include <mpi.h>
#include <cstring>
#include <omp.h>
#include <cstdio>

int Graphmpi::data[][Graphmpi::MAXN] = {}, Graphmpi::qrynode[] = {}, Graphmpi::qrydest[] = {}, Graphmpi::length[] = {};

Graphmpi& Graphmpi::getinstance() {
    static Graphmpi gm;
    return gm;
}

std::pair<int, int> Graphmpi::init(int _threadcnt, Graph* _graph) {
    threadcnt = _threadcnt;
    graph = _graph;
    int provided;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    starttime = get_wall_time();
    blocksize = (graph->v_cnt + comm_sz - 1) / comm_sz;
    int k = comm_sz - my_rank - 1;
    /*global_vertex = mynodel = blocksize * k;
    mynoder = k < comm_sz - 1 ? blocksize * (k + 1) : graph->v_cnt;*/
    if (my_rank) {
        global_vertex = mynodel = mynoder = 0;
    }
    else {
        global_vertex = mynodel = 0;
        mynoder = graph->v_cnt;
    }
    idlethreadcnt = 0;
    for (int i = 0; i < MAXTHREAD; i++) lock[i].test_and_set();
    global_vertex_lock.clear();
    return std::make_pair(mynodel, mynoder);
}

long long Graphmpi::runmajor() {
    long long tot_ans = 0;
    const int /*REQ = 0, ANS = 1, */IDLE = 2, END = 3, OVERWORK = 4, REPORT = 5, SERVER = 0;
    static int recv[MAXN * MAXTHREAD], send[MAXN * MAXTHREAD];
    MPI_Request sendrqst, recvrqst;
    MPI_Status status;
    MPI_Irecv(recv, sizeof(recv), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvrqst);
    int idlenodecnt = 0;
    std::queue<int> workq;
    for (;;) {
        int testflag = 0;
        MPI_Test(&recvrqst, &testflag, &status);
        if (testflag) {
            int m;
            MPI_Get_count(&status, MPI_INT, &m);
            /*if (recv[0] == REQ) {
                int l, r;
                graph->get_edge_index(recv[1], l, r);
                //MPI_Wait(&sendrqst, &status);
                send[0] = ANS;
                memcpy(send + 1, graph->edge + l, sizeof(graph->edge[0]) * (r - l));
                MPI_Isend(send, r - l + 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &sendrqst);
            }
            else if (recv[0] == ANS) {
                int node;
#pragma omp critical
                {
                    node = requestq.front();
                    requestq.pop();
                }
                memcpy(data[node], recv + 1, sizeof(recv[0]) * (m - 1));
                data[node][m - 1] = -1;
                length[node] = m - 1;
                lock[node].clear();
                waitforans = false;
            }
            else */if (recv[0] == IDLE) {
                for (;global_vertex_lock.test_and_set(););
                bool overworkflag = global_vertex + chunksize < mynoder;
                int tmpvertex = global_vertex;
                if (overworkflag) global_vertex += chunksize;
                global_vertex_lock.clear();
                send[0] = OVERWORK;
                send[1] = overworkflag ? tmpvertex : -1;
                MPI_Isend(send, 3, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &sendrqst);
            }
            else if (recv[0] == END) {
                tot_ans = (((long long)(recv[1]) << 32) | (unsigned)recv[2]);
                for (int i = 1; i < threadcnt; i++) {
                    length[i] = -1;
                    lock[i].clear();
                }
                break;
            }
            else if (recv[0] == OVERWORK) {
                vertex[workq.front()] = recv[1];
                lock[workq.front()].clear();
                workq.pop();
            }
            else if (recv[0] == REPORT) {
                tot_ans += (((long long)(recv[1]) << 32) | (unsigned)recv[2]);
                idlenodecnt++;
            }
            MPI_Irecv(recv, sizeof(recv), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvrqst);
        }
        /*if (!waitforans && !requestq.empty()) {
            int node;
#pragma omp critical
            {
                node = requestq.front();
            }
            //MPI_Wait(&sendrqst, &status);
            send[0] = REQ;
            send[1] = qrynode[node];
            MPI_Isend(send, 2, MPI_INT, qrydest[node], SERVER, MPI_COMM_WORLD, &sendrqst);
            waitforans = true;
        }*/
        bool idleflag;
        int tmpthread;
#pragma omp critical
        {
            idleflag = !idleq.empty();
            if (idleflag) {
                tmpthread = idleq.front();
                idleq.pop();
            }
        }
        if (idleflag) {
            workq.push(tmpthread);
            send[0] = IDLE;
            MPI_Isend(send, 1, MPI_INT, 0, SERVER, MPI_COMM_WORLD, &sendrqst);
        }
        if (idlethreadcnt == threadcnt - 1) {
            idlethreadcnt = -1;
            if (my_rank) {
                //MPI_Wait(&sendrqst, &status);
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
    return tot_ans;
}

int* Graphmpi::getneighbor(int u) {
    int thread_num = omp_get_thread_num();
    qrynode[thread_num] = u;
    qrydest[thread_num] = comm_sz - 1 - (u / blocksize);
#pragma omp critical
    requestq.push(thread_num);
    for (;lock[thread_num].test_and_set(););
    return data[thread_num];
}

int Graphmpi::getdegree() {
    return length[omp_get_thread_num()];
}

bool Graphmpi::include(int u) {
    return mynodel <= u && u < mynoder;
}

Graphmpi::Graphmpi() {}

Graphmpi::~Graphmpi() {
    MPI_Finalize();
}

std::pair<int, int> Graphmpi::get_vertex_range() {
    for (;global_vertex_lock.test_and_set(););
    bool returnflag = global_vertex < mynoder;
    int tmpvertex = global_vertex;
    if (returnflag) global_vertex += chunksize;
    global_vertex_lock.clear();
    if (returnflag) return std::make_pair(tmpvertex, std::min(mynoder, tmpvertex + chunksize));
    int thread_num = omp_get_thread_num();
#pragma omp critical
    idleq.push(thread_num);
    for (;lock[thread_num].test_and_set(););
    return std::make_pair(vertex[thread_num], vertex[thread_num] + chunksize);
}

void Graphmpi::report(long long local_ans) {
#pragma omp atomic
    node_ans += local_ans;
#pragma omp atomic
    idlethreadcnt++;
}

void Graphmpi::end() {
    printf("node = %d, thread = %d, time = %f\n", my_rank, omp_get_thread_num(), get_wall_time() - starttime);
}
