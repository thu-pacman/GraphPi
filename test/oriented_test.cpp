#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>

TEST(oriented_test, patents_k4_unoriented) {
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

    int thread_num;
    double t1,t2;
    Pattern tc_pattern(4);
    tc_pattern.add_edge(0, 1);
    tc_pattern.add_edge(0, 3);
    tc_pattern.add_edge(1, 2);
    tc_pattern.add_edge(2, 3);
    tc_pattern.add_edge(0, 2);
    tc_pattern.add_edge(1, 3);
    Schedule tc_schedule(tc_pattern);
    std::vector < std::pair<int, int> > pairs;
    int multiplicity = tc_pattern.aggresive_optimize(pairs);

    thread_num = 24;
    t1 = get_wall_time();
    long long ans_conservative;
    ASSERT_NE(ans_conservative = g->pattern_matching(tc_schedule, thread_num), 0);
    printf("conservative ans : %lld\n",ans_conservative);
    t2 = get_wall_time();
    printf("general %d thread conservative time: %.6lf\n", thread_num, t2 - t1);

    tc_schedule.add_restrict(pairs);
    long long ans_aggressive;
    
    t1 = get_wall_time();
    ASSERT_NE(ans_aggressive = g->pattern_matching(tc_schedule, thread_num), 0);
    t2 = get_wall_time();
    
    ASSERT_EQ(ans_conservative, ans_aggressive * multiplicity);
    printf("general %d thread aggressive time: %.6lf\n", thread_num, t2 - t1);

    delete g;
}

TEST(oriented_test, patents_k4_oriented_1) {
    Graph *g;
    DataLoader D;
    
    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }
    
    ASSERT_EQ(D.load_data(g,my_type,path.c_str(),1),true); 

    int thread_num;
    double t1,t2;
    Pattern tc_pattern(4);
    tc_pattern.add_edge(0, 1);
    tc_pattern.add_edge(0, 3);
    tc_pattern.add_edge(1, 2);
    tc_pattern.add_edge(2, 3);
    tc_pattern.add_edge(0, 2);
    tc_pattern.add_edge(1, 3);
    Schedule tc_schedule(tc_pattern);
    std::vector < std::pair<int, int> > pairs;
    int multiplicity = tc_pattern.aggresive_optimize(pairs);

    thread_num = 24;
    t1 = get_wall_time();
    long long ans_conservative;
    ASSERT_NE(ans_conservative = g->pattern_matching(tc_schedule, thread_num), 0);
    t2 = get_wall_time();
    printf("general %d thread conservative time: %.6lf\n", thread_num, t2 - t1);

    tc_schedule.add_restrict(pairs);
    long long ans_aggressive;

    t1 = get_wall_time();
    ASSERT_NE(ans_aggressive = g->pattern_matching(tc_schedule, thread_num), 0);
    t2 = get_wall_time();
    ASSERT_EQ(ans_conservative, ans_aggressive * multiplicity);
    printf("general %d thread aggressive time: %.6lf\n", thread_num, t2 - t1);

    delete g;
}

TEST(oriented_test, patents_k4_oriented_2) {
    Graph *g;
    DataLoader D;
    
    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }
    
    ASSERT_EQ(D.load_data(g,my_type,path.c_str(),2),true); 

    int thread_num;
    double t1,t2;
    Pattern tc_pattern(4);
    tc_pattern.add_edge(0, 1);
    tc_pattern.add_edge(0, 3);
    tc_pattern.add_edge(1, 2);
    tc_pattern.add_edge(2, 3);
    tc_pattern.add_edge(0, 2);
    tc_pattern.add_edge(1, 3);
    Schedule tc_schedule(tc_pattern);
    std::vector < std::pair<int, int> > pairs;
    int multiplicity = tc_pattern.aggresive_optimize(pairs);

    thread_num = 24;
    t1 = get_wall_time();
    long long ans_conservative;
    ASSERT_NE(ans_conservative = g->pattern_matching(tc_schedule, thread_num), 0);
    t2 = get_wall_time();
    printf("general %d thread conservative time: %.6lf\n", thread_num, t2 - t1);

    tc_schedule.add_restrict(pairs);
    long long ans_aggressive;

    t1 = get_wall_time();
    ASSERT_NE(ans_aggressive = g->pattern_matching(tc_schedule, thread_num), 0);
    t2 = get_wall_time();

    ASSERT_EQ(ans_conservative, ans_aggressive * multiplicity);
    printf("general %d thread aggressive time: %.6lf\n", thread_num, t2 - t1);
    delete g;
}
