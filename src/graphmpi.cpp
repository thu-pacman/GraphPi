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
        lock[i].test_and_set();
    }
    qlock.clear();
    chunksize = CHUNK_CONST * (MAXTHREAD - 1) * comm_sz;
    if (schedule_size > 3) chunksize /= graph->e_cnt / graph->v_cnt * (schedule_size - 3);
    chunksize = std::max(chunksize, 1);
    printf("chunksize = %d\n", chunksize);
    fflush(stdout);
    min_cur = -1;
}

long long Graphmpi::runmajor() {
    long long tot_ans = 0;
    const int IDLE = 2, END = 3, OVERWORK = 4, REPORT = 5, SERVER = 0, OVERWORKSIZE = 5;
    static int recv[MESSAGE_SIZE];
    MPI_Request sendrqst, recvrqst;
    MPI_Status status;
    MPI_Irecv(recv, sizeof(recv), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvrqst);
    int idlenodecnt = 0, cur = 0, edgel, edger, kkk = chunksize, *send;
    std::queue<int> workq;
    graph->get_edge_index(0, edgel, edger);
    auto get_send = [&]() {
        send[0] = OVERWORK;
        //if (cur <= min_cur) cur = min_cur + 1;
        if (cur == graph->v_cnt) send[1] = -1;
        else {
            int chunksize = kkk - (long long)cur * kkk / graph->v_cnt;
            send[1] = cur;
            send[2] = edgel;
            send[4] = 0;
            if (edger - edgel > chunksize) send[3] = edgel += chunksize;
            else {
                send[3] = edger;
                for (int sum = edger - edgel;; send[4]++) {
                    int k = std::max(graph->v_cnt / 10, 1);
                    if (cur % k == 0) {
                        printf("nearly %d out of 10 task assigned, time = %f\n", cur / k, get_wall_time() - starttime);
                        fflush(stdout);
                    }
                    cur++;
                    if (cur < graph->v_cnt) graph->get_edge_index(cur, edgel, edger);
                    sum += edger - edgel;
                    if (sum > chunksize || cur == graph->v_cnt) break;
                }
            }
        }
    };
    auto roll_send = [&]() {
        static int i = 0, buf[ROLL_SIZE][OVERWORKSIZE + 1];
        send = buf[i];
        i = (i + 1) % ROLL_SIZE;
    };
    for (;;) {
        int testflag = 0;
        MPI_Test(&recvrqst, &testflag, &status);
        if (testflag) {
            if (recv[0] == IDLE) {
                roll_send();
                if (min_cur < recv[1]) {
                    min_cur = recv[1];
                }
                get_send();
                MPI_Isend(send, OVERWORKSIZE, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &sendrqst);
            }
            else if (recv[0] == END) {
                tot_ans = (((long long)(recv[1]) << 32) | (unsigned)recv[2]);
                break;
            }
            else if (recv[0] == OVERWORK) {
                memcpy(data[workq.front()], recv, OVERWORKSIZE * sizeof(recv[0]));
                lock[workq.front()].clear();
                workq.pop();
            }
            else if (recv[0] == REPORT) {
                tot_ans += (((long long)(recv[1]) << 32) | (unsigned)recv[2]);
                idlenodecnt++;
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
        if(tmpflag) {
            if (my_rank) {
                workq.push(tmpthread);
                roll_send();
                send[0] = IDLE;
                send[1] = min_cur;
                MPI_Isend(send, 2, MPI_INT, 0, SERVER, MPI_COMM_WORLD, &sendrqst);
            }
            else {
                get_send();
                memcpy(data[tmpthread], send, OVERWORKSIZE * sizeof(send[0]));
                lock[tmpthread].clear();
            }
        }
        if (idlethreadcnt == threadcnt - 1) {
            idlethreadcnt = -1;
            if (my_rank) {
                roll_send();
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
                roll_send();
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

void Graphmpi::set_cur(int i) {
    if (i > min_cur) {
        min_cur = i;
    }
}
