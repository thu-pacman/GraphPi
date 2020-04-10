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
    int thread_num = 24;
    long long tri_cnt = 7515023;
    double t1,t2;
    
    bool is_pattern_valid;
    int performance_modeling_type = 0;
    int restricts_type = 0;
    bool use_in_exclusion_optimize = false;
    
    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
    assert(is_pattern_valid);
    
    std::vector< std::vector< std::pair<int,int> > >restricts;
    schedule.restricts_generate(schedule.get_adj_mat_ptr(), restricts);
    
    schedule.print_schedule();

    for( auto& pairs : restricts) {
        Schedule cur_schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
        std::sort(pairs.begin(), pairs.end());
        cur_schedule.add_restrict(pairs);
        double t1 = get_wall_time();
        long long ans = g->pattern_matching(cur_schedule, thread_num);
        double t2 = get_wall_time();
        printf("ans %lld time %.6lf\n", ans, t2 - t1);
        printf("%d ", pairs.size());
        for(const auto& p : pairs)
            printf("(%d,%d)", p.first, p.second);
        puts("");
    }
    {
        std::vector< std::pair<int,int> > pairs;
        Schedule cur_schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
        cur_schedule.GraphZero_aggressive_optimize(pairs);
        cur_schedule.add_restrict(pairs);
        double t1 = get_wall_time();
        long long ans = g->pattern_matching(cur_schedule, thread_num);
        double t2 = get_wall_time();
        printf("ans %lld time %.6lf\n", ans, t2 - t1);
        printf("%d ", pairs.size());
        for(const auto& p : pairs)
            printf("(%d,%d)", p.first, p.second);
        puts("");
    }
    {
        std::vector< std::pair<int,int> > pairs;
        Schedule cur_schedule(pattern, is_pattern_valid, 0, 1, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
        double t1 = get_wall_time();
        long long ans = g->pattern_matching(cur_schedule, thread_num);
        double t2 = get_wall_time();
        printf("ans %lld time %.6lf\n", ans, t2 - t1);
        pairs = cur_schedule.restrict_pair;
        printf("%d ", pairs.size());
        for(const auto& p : pairs)
            printf("(%d,%d)", p.first, p.second);
        puts("");
    }
}

int main(int argc,char *argv[]) {
    Graph *g;
    DataLoader D;

    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }

    assert(D.load_data(g,my_type,path.c_str())==true); 

    printf("Load data success!\n");
    fflush(stdout);

    Pattern pattern(5);
    pattern.add_edge(0, 1);
    pattern.add_edge(0, 4);
    pattern.add_edge(1, 2);
    pattern.add_edge(1, 3);
    pattern.add_edge(2, 3);
    pattern.add_edge(2, 4);
    test_pattern(g, pattern);

    delete g;
}
