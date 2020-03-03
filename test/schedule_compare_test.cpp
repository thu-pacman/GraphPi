#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>
#include <algorithm>

std::vector<long long> graph_degree_info;
std::vector<long long> graph_size_info;

void test_pattern(Graph* g, PatternType type) {
    Pattern pattern(type);
    
    printf("test pattern : ");
    PatternType_printer(type);

    bool is_pattern_valid;
    int performance_modeling_type;
    
    performance_modeling_type = 1;
    Schedule schedule_our(pattern, is_pattern_valid, performance_modeling_type, graph_degree_info, graph_size_info);
    ASSERT_EQ(is_pattern_valid, true);

    performance_modeling_type = 2;
    Schedule schedule_gz(pattern, is_pattern_valid, performance_modeling_type, graph_degree_info, graph_size_info);
    ASSERT_EQ(is_pattern_valid, true);

    if( is_equal_adj_mat( schedule_our.get_adj_mat_ptr(), schedule_gz.get_adj_mat_ptr(), pattern.get_size())) {
        printf("same schedule\n");
        return;
    }

    std::vector< std::pair<int,int> > gz_pairs;
    schedule_gz.GraphZero_aggressive_optimize( schedule_gz.get_adj_mat_ptr(), gz_pairs);
    schedule_gz.add_restrict(gz_pairs);

    std::vector< std::pair<int,int> > our_pairs;
    schedule_our.aggressive_optimize( schedule_our.get_adj_mat_ptr(), our_pairs);
    schedule_our.add_restrict(our_pairs);

    int thread_num = 24;
    double t1,t2,t3,t4;

    t1 = get_wall_time();
    long long ans_our = g->pattern_matching(schedule_our, thread_num);
    t2 = get_wall_time();

    t3 = get_wall_time();
    long long ans_gz = g->pattern_matching(schedule_gz, thread_num);
    t4 = get_wall_time();

    printf("our ans: %lld time: %.6lf\n", ans_our, t2 - t1);
    printf("GZ  ans: %lld time: %.6lf\n", ans_gz, t4 - t3);
}

TEST(schedule_compare_test, schedule_compare_patents) {
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
    
    Pattern pattern(PatternType::House);

    ASSERT_EQ(D.load_data(g,my_type,path.c_str(), pattern_size, max_pattern_degree, pattern_diameter, graph_degree_info, graph_size_info),true); 
    
    printf("Load data success!\n");
    fflush(stdout);

    test_pattern(g, PatternType::Rectangle);
    test_pattern(g, PatternType::Pentagon);
    test_pattern(g, PatternType::House);
    test_pattern(g, PatternType::Hourglass);
    test_pattern(g, PatternType::Cycle_6_Tri);
    test_pattern(g, PatternType::Clique_7_Minus);

    delete g;
}
