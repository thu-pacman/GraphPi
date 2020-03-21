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

void test_pattern(Graph* g, Pattern &pattern) {
    std::vector<double> speed_up;
    speed_up.clear();

    int thread_num = 24;
    double t1,t2,t3,t4;

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

        Schedule schedule_our(cur_pattern, is_pattern_valid, 0, 1, false, g->v_cnt, g->e_cnt, g->tri_cnt);
        
        if(is_pattern_valid == false) continue;
        
        Schedule schedule_gz(cur_pattern, is_pattern_valid, 0, 2, false, g->v_cnt, g->e_cnt, g->tri_cnt);

        const auto& our_pairs = schedule_our.restrict_pair;
        const auto& gz_pairs = schedule_gz.restrict_pair;

        bool tag = true;
        for(const auto& gz_p : gz_pairs) {
            bool flag = false;
            for(const auto& our_p : our_pairs)
                if(gz_p == our_p) {
                    flag = true;
                    break;
                }
            if(!flag) {
                tag = false;
                break;
            }
        }

        if(tag) continue;

        t1 = get_wall_time();
        long long ans = g->pattern_matching(schedule_our, thread_num);
        t2 = get_wall_time();
        
        t3 = get_wall_time();
        long long ans2 = g->pattern_matching(schedule_gz, thread_num);
        t4 = get_wall_time();

        printf("%lld,%lld,%.6lf,%.6lf\n", ans, ans2, t2 - t1, t4 - t3);
        schedule_our.print_schedule();
        for(auto& p : our_pairs)
            printf("(%d,%d)", p.first, p.second);
        puts("");
        for(auto& p : gz_pairs)
            printf("(%d,%d)", p.first, p.second);
        puts("");
        
        fflush(stdout);
        
        speed_up.push_back((t4-t3)/(t2-t1));

    } while( std::next_permutation(rank, rank + size));
    
    double cnt = 0;
    double mx = -1;
    for(const auto& s : speed_up) {
        cnt += s;
        mx = std::max(mx, s);
    }
    printf("%u\n", speed_up.size());
    if(speed_up.size() == 0) return;
    printf("%.6lf %.6lf\n", cnt / speed_up.size(), mx);
}

int main(int argc,char *argv[]) {
    Graph *g;
    DataLoader D;

    const std::string data_type = argv[1];
    const std::string path = argv[2];
    
    int size = atoi(argv[3]);
    char* adj_mat = argv[4];
    
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
