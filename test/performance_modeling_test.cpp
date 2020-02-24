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

    // These variables are used in performance modeling,
    // but performance_modeling is not acceptable now.
    int pattern_size = 5;
    int pattern_diameter = 2;
    int max_pattern_degree = 3;
    std::vector<long long> graph_degree_info;
    std::vector<long long> graph_size_info;
    
    Pattern pattern(pattern_size);
    pattern.add_edge(0, 1);
    pattern.add_edge(0, 4);
    pattern.add_edge(1, 2);
    pattern.add_edge(2, 3);
    pattern.add_edge(2, 4);
    pattern.add_edge(3, 4);

    const int* adj_mat = pattern.get_adj_mat_ptr();

    printf("original adj_mat\n");
    for(int i = 0; i < pattern_size; ++i) {
        for(int j = 0; j < pattern_size; ++j)
            printf("%d",adj_mat[INDEX(i,j,pattern_size)]);
        puts("");
    }

    ASSERT_EQ(D.load_data(g,my_type,path.c_str(), pattern_size, max_pattern_degree, pattern_diameter, graph_degree_info, graph_size_info),true); 
    
    printf("Load data success!\n");
    fflush(stdout);

    bool is_pattern_valid = false;
    bool use_performance_modeling = true;
    Schedule schedule(pattern, is_pattern_valid, use_performance_modeling, graph_degree_info, graph_size_info);
    
    ASSERT_EQ(is_pattern_valid, true);

    adj_mat = schedule.get_adj_mat_ptr();
    printf("performance modeling adj_mat\n");
    for(int i = 0; i < pattern_size; ++i) {
        for(int j = 0; j < pattern_size; ++j)
            printf("%d",adj_mat[INDEX(i,j,pattern_size)]);
        puts("");
    }

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
