#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>
#include <algorithm>

TEST(performance_modeling_test, performance_modeling_patents_qg3) {
    Graph *g;
    DataLoader D;
    
    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }

    ASSERT_EQ(D.load_data(g,my_type,path.c_str()),true);
    
    printf("Load data success!\n");
    fflush(stdout);
    
    Pattern pattern(PatternType::House);

    bool is_pattern_valid = false;
    int performance_modeling_type = 1;
    bool use_in_exclusion_optimize = false;
    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt);
    ASSERT_EQ(is_pattern_valid, true);

    printf("performance modeling adj_mat\n");
    schedule.print_schedule();


    std::vector< std::pair<int,int> > pairs;
    schedule.aggressive_optimize(pairs);
    schedule.add_restrict(pairs);

    printf("aggressive optimize success!\n");
    fflush(stdout);

    int thread_num = 24;
    double t1,t2;
    long long ans;
    
    printf("pattern_matching begin\n");
    fflush(stdout);
    
    t1 = get_wall_time();
    ans = g->pattern_matching(schedule, thread_num);
    t2 = get_wall_time();

    printf("ans: %lld time: %.6lf\n", ans, t2 - t1);

    delete g;
}
