#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>
#include <algorithm>

void test_pattern(Graph* g, PatternType type) {
    Pattern pattern(type);
    int thread_num = 24;
    double t1,t2,t3,t4;
    
    printf("test pattern : ");
    PatternType_printer(type);

    bool is_pattern_valid;
    int performance_modeling_type;
    bool use_in_exclusion_optimize = false;
    
    performance_modeling_type = 1;
    Schedule schedule_our(pattern, is_pattern_valid, performance_modeling_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt);
    ASSERT_EQ(is_pattern_valid, true);

    performance_modeling_type = 2;
    Schedule schedule_gz(pattern, is_pattern_valid, performance_modeling_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt);
    ASSERT_EQ(is_pattern_valid, true);

    std::vector< std::pair<int,int> > gz_pairs;
    schedule_gz.GraphZero_aggressive_optimize(gz_pairs);
    schedule_gz.add_restrict(gz_pairs);

    std::vector< std::vector< std::pair<int,int> > >restricts;
    schedule_our.restricts_generate(schedule_our.get_adj_mat_ptr(), restricts);
    printf("res size = %d\n", restricts.size());

    std::vector< std::pair<int,int> > our_pairs;
    schedule_our.restrict_selection(g->v_cnt, g->e_cnt, restricts, our_pairs);
    schedule_our.add_restrict(our_pairs);
   
    t3 = get_wall_time();
    long long ans_gz = g->pattern_matching(schedule_gz, thread_num);
    t4 = get_wall_time();
    
    t1 = get_wall_time();
    long long ans_our = g->pattern_matching(schedule_our, thread_num);
    t2 = get_wall_time();

    printf("our ans: %lld time: %.6lf\n", ans_our, t2 - t1);
    schedule_our.print_schedule();
    for(int i = 0; i < our_pairs.size(); ++i)
        printf("(%d,%d)",our_pairs[i].first, our_pairs[i].second);
    puts("");

    printf("GZ  ans: %lld time: %.6lf\n", ans_gz, t4 - t3);
    schedule_gz.print_schedule();
    for(int i = 0; i < gz_pairs.size(); ++i)
        printf("(%d,%d)",gz_pairs[i].first, gz_pairs[i].second);
    puts("");
    fflush(stdout);
}

TEST(performance_compare_test, performance_compare_patents) {
    Graph *g;
    DataLoader D;
    
    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }

    Pattern pattern(PatternType::House);

    ASSERT_EQ(D.load_data(g,my_type,path.c_str()),true); 
    
    printf("Load data success!\n");
    fflush(stdout);

    test_pattern(g, PatternType::Cycle_6_Tri);
    test_pattern(g, PatternType::Cycle_6_Tri);
    test_pattern(g, PatternType::Cycle_6_Tri);
//    test_pattern(g, PatternType::Rectangle);
//    test_pattern(g, PatternType::QG3);
//    test_pattern(g, PatternType::Pentagon);
//    test_pattern(g, PatternType::House);
//    test_pattern(g, PatternType::Hourglass);
//    test_pattern(g, PatternType::Clique_7_Minus);

    delete g;
}
