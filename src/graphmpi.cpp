#include "../include/graphmpi.h"
#include "../include/common.h"
#include <mpi.h>
#include <cstring>
#include <omp.h>
#include <cstdio>

int Graphmpi::data[][Graphmpi::MAXN] = {}, Graphmpi::qrynode[] = {}, Graphmpi::qrydest[] = {}, Graphmpi::length[] = {};
double starttime;

Graphmpi& Graphmpi::getinstance() {
    static Graphmpi gm;
    return gm;
}

std::pair<int, int> Graphmpi::init(int _threadcnt, Graph* _graph) {
    starttime = get_wall_time();
    threadcnt = _threadcnt;
    graph = _graph;
    int provided;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    blocksize = (graph->v_cnt + comm_sz - 1) / comm_sz;
    int k = comm_sz - my_rank - 1;
    global_vertex = mynodel = blocksize * k;
    mynoder = k < comm_sz - 1 ? blocksize * (k + 1) : graph->v_cnt;
    idlethreadcnt = 0;
    for (int i = 0; i < MAXTHREAD; i++) lock[i].test_and_set();
    return std::make_pair(mynodel, mynoder);
}

int Graphmpi::runmajor() {
    long long tot_ans = 0;
    const int REQ = 0, ANS = 1, IDLE = 2, END = 3, OVERWORK = 4, SERVER = 0;
    static int recv[MAXN * MAXTHREAD], send[MAXN * MAXTHREAD];
    MPI_Request sendrqst, recvrqst;
    MPI_Status status;
    MPI_Irecv(recv, sizeof(recv), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvrqst);
    int idlenodecnt = 0;
    bool waitforans = false, idleflag[MAXTHREAD];
    memset(idleflag, false, sizeof(idleflag));
    for (;;) {
        int testflag = 0;
        MPI_Test(&recvrqst, &testflag, &status);
        if (testflag) {
            int m;
            MPI_Get_count(&status, MPI_INT, &m);
            if (recv[0] == REQ) {
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
            else if (recv[0] == IDLE) {
                tot_ans += ((long long)(recv[1]) << 32) | recv[2];
                bool overworkflag = false;
                int tmpvertex;
#pragma omp critical
                if (global_vertex + threadcnt - 1 < mynoder) {
                    overworkflag = true;
                    global_vertex += threadcnt - 1;
                    tmpvertex = global_vertex;
                }
                if (overworkflag) {
                    send[0] = OVERWORK;
                    int j = 1;
                    for (int i = 1; i < threadcnt; i++) {
                        /*int l, r;
                        graph->get_edge_index(global_vertex - i, l, r);
                        send[j] = global_vertex - i;
                        send[j + 1] = r - l;
                        memcpy(send + j + 2, graph->edge + l, sizeof(send[0]) * (r - l));
                        j += r - l + 2;*/
                        send[j] = tmpvertex - i;
                        j++;
                    }
                    MPI_Isend(send, j, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &sendrqst);
                }
                else {
                    idlenodecnt += !idleflag[status.MPI_SOURCE];
                    idleflag[status.MPI_SOURCE] = true;
                }
            }
            else if (recv[0] == END) {
                tot_ans = (((long long)(recv[1]) << 32) | recv[2]);
                for (int i = 1; i < threadcnt; i++) {
                    length[i] = -1;
                    lock[i].clear();
                }
                break;
            }
            else if (recv[0] == OVERWORK) {
                //int j = 1;
                for (int i = 1; i < threadcnt; i++) {
                    /*vertex[i] = recv[j];
                    length[i] = recv[j + 1];
                    memcpy(data[i], recv + j + 2, sizeof(recv[0]) * recv[j + 1]);
                    lock[i].clear();
                    j += recv[j + 1] + 2;*/
                    vertex[i] = recv[i];
                    lock[i].clear();
                }
            }
            MPI_Irecv(recv, sizeof(recv), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvrqst);
        }
        if (!waitforans && !requestq.empty()) {
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
        }
        bool tmpflag;
#pragma omp critical
        tmpflag = (idlethreadcnt == threadcnt - 1);
        if (tmpflag) {
            idlethreadcnt = 0;
            idlenodecnt += !idleflag[0];
            idleflag[0] = true;
            if (my_rank) {
                //MPI_Wait(&sendrqst, &status);
                send[0] = IDLE;
                send[1] = node_ans >> 32;
                send[2] = node_ans & ((1ll << 32) - 1ll);
                node_ans = 0;
                MPI_Isend(send, 3, MPI_INT, 0, SERVER, MPI_COMM_WORLD, &sendrqst);
            }
            else {
                tot_ans += node_ans;
                node_ans = 0;
            }
        }
        if (idlenodecnt == comm_sz) {
            for (int i = 1; i < comm_sz; i++) {
                //MPI_Wait(&sendrqst, &status);
                send[0] = END;
                send[1] = tot_ans >> 32;
                send[2] = tot_ans & ((1ll << 32) - 1ll);
                MPI_Isend(send, 3, MPI_INT, i, SERVER, MPI_COMM_WORLD, &sendrqst);
            }
            break;
        }
    }
    return tot_ans;
}

int* Graphmpi::getneighbor(int u) {
    qrynode[omp_get_thread_num()] = u;
    qrydest[omp_get_thread_num()] = comm_sz - 1 - (u / blocksize);
#pragma omp critical
    {
        requestq.push(omp_get_thread_num());
    }
    for (;lock[omp_get_thread_num()].test_and_set(););
    return data[omp_get_thread_num()];
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

bool Graphmpi::get_startvertex(int &_vertex, int *&_data, int &size, long long &local_ans) {
    bool returnflag = false;
#pragma omp critical
    if (global_vertex < mynoder) {
        _vertex = global_vertex;
        _data = nullptr;
        global_vertex++;
        returnflag = true;
    }
    if (returnflag) return true;
#pragma omp atomic
    node_ans += local_ans;
#pragma omp atomic
    idlethreadcnt++;
    local_ans = 0;
    if (!my_rank) return false;
    for (;lock[omp_get_thread_num()].test_and_set(););
    _vertex = vertex[omp_get_thread_num()];
    //_data = data[omp_get_thread_num()];
    _data = nullptr;
    size = length[omp_get_thread_num()];
    return ~size ? true : false;
}
