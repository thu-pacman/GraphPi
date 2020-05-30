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

void Graphmpi::init(int _threadcnt, Graph* _graph, const Schedule& schedule) {
    initialized = true;
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
        data[i] = new unsigned int[MESSAGE_SIZE];
        lock[i].test_and_set();
    }
    const int CHUNK_CONST = 70;
    omp_chunk_size = std::max(int((long long)(graph->v_cnt) * CHUNK_CONST / graph->e_cnt), 8);
    mpi_chunk_size = (threadcnt - 1) * omp_chunk_size;
    skip_flag = ~schedule.get_restrict_last(1);
    printf("mpi_csize = %d, omp_csize = %d\n", mpi_chunk_size, omp_chunk_size);
    fflush(stdout);
}

long long Graphmpi::runmajor() {
    long long tot_ans = 0;
    const int IDLE = 2, END = 3, OVERWORK = 4, REPORT = 5, SERVER = 0, ROLL_SIZE = 327680;
    static unsigned int recv[MESSAGE_SIZE], local_data[MESSAGE_SIZE];
    MPI_Request sendrqst, recvrqst;
    MPI_Status status;
    MPI_Irecv(recv, sizeof(recv), MPI_UNSIGNED, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvrqst);
    int idlenodecnt = 0, cur = 0;
    unsigned int edgel, edger, *send;
    std::queue<int> workq;
    graph->get_edge_index(0, edgel, edger);
    auto get_send = [&](unsigned int *send) {
        auto next_cur = [&]() {
            cur++;
            int k = std::max(graph->v_cnt / 100, 1);
            if (cur % k == 0) {
                printf("nearly %d out of 100 task assigned, time = %f\n", cur / k, get_wall_time() - starttime);
                fflush(stdout);
            }
            if (cur < graph->v_cnt) {
                graph->get_edge_index(cur, edgel, edger);
            }
        };
        send[0] = OVERWORK;
        if (cur < graph->v_cnt && skip_flag && graph->edge[edgel] >= cur) next_cur();
        if (cur == graph->v_cnt) send[1] = -1;
        else {
            send[1] = cur;
            send[2] = edgel;
            if (edger - edgel > mpi_chunk_size) send[3] = edgel += mpi_chunk_size;
            else {
                send[3] = edger;
                next_cur();
            }
        }
    };
    int buft = 0, bufw = 0;
    static unsigned int buf[ROLL_SIZE][MESSAGE_SIZE + 1];
    auto roll_send = [&]() {
        send = buf[bufw] + 1;
        bufw = (bufw + 1) % ROLL_SIZE;
    };
    auto update_send = [&]() {
        if (buft != bufw) {
            static bool ini_flag = false;
            bool flag;
            if (ini_flag) {
                int testflag;
                MPI_Test(&sendrqst, &testflag, MPI_STATUS_IGNORE);
                flag = ini_flag;
            }
            else ini_flag = flag = true;
            if (flag) {
                MPI_Isend(buf[buft] + 1, MESSAGE_SIZE, MPI_UNSIGNED, buf[buft][0], 0, MPI_COMM_WORLD, &sendrqst);
                buft = (buft + 1) % ROLL_SIZE;
            }
        }
    };
    auto get_new_local_data = [&]() {
        if (my_rank) {
            roll_send();
            send[-1] = SERVER;
            send[0] = IDLE;
        }
        else get_send(local_data);
    };
    get_new_local_data();
    for (;;) {
        update_send();
        int testflag = 0;
        MPI_Test(&recvrqst, &testflag, &status);
        if (testflag) {
            if (recv[0] == IDLE) {
                roll_send();
                send[-1] = status.MPI_SOURCE;
                get_send(send);
            }
            else if (recv[0] == OVERWORK) {
                memcpy(local_data, recv, sizeof(recv));
            }
            else if (recv[0] == END) {
                tot_ans = (((long long)(recv[1]) << 32) | (unsigned)recv[2]);
                break;
            }
            else if (recv[0] == REPORT) {
                tot_ans += (((long long)(recv[1]) << 32) | (unsigned)recv[2]);
                idlenodecnt++;
            }
            MPI_Irecv(recv, sizeof(recv), MPI_UNSIGNED, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvrqst);
        }
        if (!idleq.empty()) {
            if (local_data[1] == -1) {
                int tmpthread = idleq.front_and_pop();
                data[tmpthread][1] = -1;
                lock[tmpthread].clear();
            }
            else if (local_data[2] != local_data[3]) {
                int tmpthread = idleq.front_and_pop();
                memcpy(data[tmpthread], local_data, sizeof(local_data));
                data[tmpthread][3] = local_data[2] = std::min(local_data[2] + omp_chunk_size, local_data[3]);
                lock[tmpthread].clear();
                if (local_data[2] == local_data[3]) get_new_local_data();
            }
        }
        if (idlethreadcnt == threadcnt - 1) {
            idlethreadcnt = -1;
            if (my_rank) {
                roll_send();
                send[-1] = SERVER;
                send[0] = REPORT;
                send[1] = node_ans >> 32;
                send[2] = node_ans;
            }
            else {
                idlenodecnt++;
                tot_ans += node_ans;
            }
        }
        if (idlenodecnt == comm_sz) {
            for (int i = 1; i < comm_sz; i++) {
                roll_send();
                send[-1] = i;
                send[0] = END;
                send[1] = tot_ans >> 32;
                send[2] = tot_ans;
            }
            for (; buft != bufw; update_send());
            if (comm_sz > 1) MPI_Wait(&sendrqst, MPI_STATUS_IGNORE);
            break;
        }
    }
    return tot_ans;
}

Graphmpi::Graphmpi() {
    initialized = false;    
}

Graphmpi::~Graphmpi() {
    if (initialized) {
        for (int i = 0; i < threadcnt; i++) delete[] data[i];
        MPI_Finalize();
    }
}

unsigned int* Graphmpi::get_edge_range() {
    int thread_num = omp_get_thread_num();
    idleq.push(thread_num);
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

void Graphmpi::set_loop_flag() {
    loop_flag = true;
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

Bx2k256Queue::Bx2k256Queue() {
    memset(q, -1, sizeof(q));
    h = t = 0;
    lock.clear();
}

bool Bx2k256Queue::empty() {
    bool ans;
    for (;lock.test_and_set(););
    ans = (h == t);
    lock.clear();
    return ans;
}

void Bx2k256Queue::push(int k) {
    for (;lock.test_and_set(););
    q[t] = k;
    t++;
    lock.clear();
}

int Bx2k256Queue::front_and_pop() {
    int ans;
    for (;lock.test_and_set(););
    ans = q[h++];
    lock.clear();
    return ans;
}
