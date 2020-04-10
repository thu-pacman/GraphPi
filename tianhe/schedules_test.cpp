#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"
#include "../include/motif_generator.h"

#include <assert.h>
#include <iostream>
#include <string>
#include <algorithm>

int node_cnt = 72;
int my_rank;

void test_pattern(Graph* g, Pattern &pattern) {
    int work_cnt = 0;

    int thread_num = 24;
    double t3,t4;

    bool is_pattern_valid;
    int performance_modeling_type;
    bool use_in_exclusion_optimize;

    int size = pattern.get_size();
    const int* adj_mat = pattern.get_adj_mat_ptr();
    int rank[size];
    for(int i = 0; i < size; ++i) rank[i] = i;
    do{
        Pattern cur_pattern(size);
        for(int i = 0; i < size; ++i)
            for(int j = 0; j < i; ++j)
                if( adj_mat[INDEX(i,j,size)])
                    cur_pattern.add_edge(rank[i],rank[j]);

        bool valid = true;
        const int* cur_adj_mat = cur_pattern.get_adj_mat_ptr();
        for(int i = 1; i < size; ++i) {
            bool have_edge = false;
            for(int j = 0; j < i; ++j)
                if(cur_adj_mat[INDEX(i,j,size)]) {
                    have_edge = true;
                    break;
                }
            if(!have_edge) {
                valid = false;
                break;
            }
        }
        if(!valid) continue;

        Schedule schedule_gz(cur_pattern, is_pattern_valid, 0, 2, false, g->v_cnt, g->e_cnt, g->tri_cnt);
        if(is_pattern_valid == false) continue;

        ++work_cnt;
        if( work_cnt % node_cnt != my_rank) continue;

        const auto& gz_pairs = schedule_gz.restrict_pair;

        t3 = get_wall_time();
        long long ans2 = g->pattern_matching(schedule_gz, thread_num);
        t4 = get_wall_time();
        int num = schedule_gz.get_in_exclusion_optimize_num_when_not_optimize();

        printf("%lld,%d,%.6lf\n", ans2, num, t4 - t3);
        schedule_gz.print_schedule();
        for(auto& p : gz_pairs)
            printf("(%d,%d)", p.first, p.second);
        puts("");
        
        fflush(stdout);
    } while( std::next_permutation(rank, rank + size));
    
}

int main(int argc,char *argv[]) {
    Graph *g;
    DataLoader D;

    const std::string data_type = argv[1];
    const std::string path = argv[2];
    
    int size = atoi(argv[3]);
    char* adj_mat = argv[4];
    my_rank = atoi(argv[5]);
    
    DataType my_type;

    GetDataType(my_type, data_type);

    if( my_type == DataType::Invalid) {
        printf("Dataset not found!\n");
        return 0;
    }

    assert(D.load_data(g,my_type,path.c_str())==true); 

    printf("Load data success!\n");
    fflush(stdout);

    Pattern p(size, adj_mat);
    test_pattern(g, p);
    
    delete g;
}
