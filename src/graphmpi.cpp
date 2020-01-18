#include "../include/graphmpi.h"
#include <mpi.h>
#include <cstring>
#include <omp.h>
#include <cstdio>

int Graphmpi::data[][Graphmpi::MAXN] = {}, Graphmpi::qrynode[] = {}, Graphmpi::qrydest[] = {}, Graphmpi::length[] = {};

Graphmpi& Graphmpi::getinstance() {
    static Graphmpi gm;
    return gm;
}

std::pair<int, int> Graphmpi::init(int threadcnt, Graph* _graph) {
    graph = _graph;
    int provided;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    blocksize = (graph->v_cnt + comm_sz - 1) / comm_sz;
    mynodel = blocksize * my_rank;
    mynoder = my_rank < comm_sz - 1 ? blocksize * (my_rank + 1) : graph->v_cnt;
    idlethreadcnt = threadcnt - 1;
    for (int i = 0; i < MAXTHREAD; i++) lock[i].test_and_set();
    return std::make_pair(mynodel, mynoder);
}

int Graphmpi::runmajor() {
    long long tot_ans = 0;
    const int REQ = 0, ANS = 1, IDLE = 2, END = 3;
    static int recv[MAXN], send[MAXN];
    MPI_Request sendrqst, recvrqst;
    MPI_Status status;
    MPI_Irecv(recv, sizeof(recv), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvrqst);
    int idlenodecnt = 0;
    bool waitforans = false;
    for (;;) {
        int testflag = 0;
        MPI_Test(&recvrqst, &testflag, &status);
        if (testflag) {
            int m;
            MPI_Get_count(&status, MPI_INT, &m);
            if (recv[0] == REQ) {
                int l, r;
                graph->get_edge_index(recv[1], l, r);
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
            else if (recv[0] == IDLE) {
                idlenodecnt++;
                tot_ans += ((recv[1] << 30) | recv[2]);
            }
            else if (recv[0] == END) {
                tot_ans = ((recv[1] << 30) | recv[2]);
                break;
            }
            MPI_Irecv(recv, sizeof(recv), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvrqst);
        }
        if (!waitforans && !requestq.empty()) {
            send[0] = REQ;
            int node;
#pragma omp critical
            {
                node = requestq.front();
            }
            send[1] = qrynode[node];
            MPI_Isend(send, 2, MPI_INT, qrydest[node], 0, MPI_COMM_WORLD, &sendrqst);
            waitforans = true;
        }
        if (!idlethreadcnt) {
            idlethreadcnt = -1;
            idlenodecnt++;
            if (my_rank) {
                send[0] = IDLE;
                send[1] = node_ans >> 30;
                send[2] = node_ans & ((1ll << 30) - 1);
                MPI_Isend(send, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, &sendrqst);
            }
            else {
#pragma omp atomic
                tot_ans += node_ans;
            }
        }
        if (idlenodecnt == comm_sz) {
            for (int i = 1; i < comm_sz; i++) {
                send[0] = END;
                send[1] = tot_ans >> 30;
                send[2] = tot_ans & ((1ll << 30) - 1);
                MPI_Isend(send, 3, MPI_INT, i, 0, MPI_COMM_WORLD, &sendrqst);
            }
            break;
        }
    }
    return tot_ans;
}

int* Graphmpi::getneighbor(int u) {
    int my_thread_num = omp_get_thread_num();
    qrynode[my_thread_num] = u;
    qrydest[my_thread_num] = u / blocksize;
#pragma omp critical
    {
        requestq.push(my_thread_num);
    }
    for (;lock[my_thread_num].test_and_set(););
    return data[my_thread_num];
}

int Graphmpi::getdegree() {
    return length[omp_get_thread_num()];
}

void Graphmpi::idle(int ans) {
#pragma omp atomic
    node_ans += ans;
#pragma omp atomic
    idlethreadcnt--;
}

bool Graphmpi::include(int u) {
    return mynodel <= u && u < mynoder;
}

Graphmpi::Graphmpi() {}

Graphmpi::~Graphmpi() {
    MPI_Finalize();
}
