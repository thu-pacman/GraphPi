#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>
#include <algorithm>

TEST(pattern_matching_test, pattern_matching_patents_triangle) {
    Graph *g;
    DataLoader D;
    
    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }

    // These variables are used in performance modeling,
    // but performance_modeling is not acceptable now.
    int pattern_size = 3;
    int pattern_diameter = 1;
    int max_pattern_degree = 2;
    std::vector<long long> graph_degree_info;
    std::vector<long long> graph_size_info;
    
    Pattern pattern(PatternType::Hourglass);

    ASSERT_EQ(D.load_data(g,my_type,path.c_str(), pattern_size, max_pattern_degree, pattern_diameter, graph_degree_info, graph_size_info),true); 
    
    printf("Load data success!\n");
    fflush(stdout);

    bool is_pattern_valid = false;
    bool use_performance_modeling = false;
    Schedule schedule(pattern, is_pattern_valid, use_performance_modeling, graph_degree_info, graph_size_info);

    ASSERT_EQ(is_pattern_valid, true);

    std::vector< std::pair<int,int> > pairs;
    schedule.aggressive_optimize( schedule.get_adj_mat_ptr(), pairs);

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
