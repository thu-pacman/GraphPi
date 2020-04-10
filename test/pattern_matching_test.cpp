#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>
#include <algorithm>

TEST(pattern_matching_test, pattern_matching_patents_Cycle6Tri) {
    Graph *g;
    DataLoader D;
    
    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }

    ASSERT_EQ(D.load_data(g,my_type,path.c_str()), true);
    
    printf("Load data success!\n");
    fflush(stdout);
    
    Pattern pattern(PatternType::Cycle_6_Tri);

    bool is_pattern_valid = false;
    int performance_modeling_type = 0;
    bool use_in_exclusion_optimize = false;
    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt);
    ASSERT_EQ(is_pattern_valid, true);

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
