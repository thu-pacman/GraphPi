#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>

TEST(oriented_test, patents_hourglass_oriented_1) {
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

    int dataset_vertex_size = 3072441;
    int dataset_edge_size = 117185083;
    int size = 6;

    Pattern pattern(size);
    pattern.add_edge(0, 1);
    pattern.add_edge(1, 2);
    pattern.add_edge(2, 3);
    pattern.add_edge(0, 3);
    pattern.add_edge(0, 4);
    pattern.add_edge(1, 4);
    pattern.add_edge(2, 5);
    pattern.add_edge(3, 5);
    pattern.add_edge(4, 5);

    bool is_valid;
    Schedule schedule(pattern, is_valid, true, dataset_vertex_size, dataset_edge_size);
    
    std::vector< std::pair<int,int> > pairs;
    int multi = schedule.aggresive_optimize( schedule.get_adj_mat_ptr(), pairs);
    schedule.add_restrict(pairs);

    int thread_num = 24;
    double t1,t2;
    long long ans;
    
    t1 = get_wall_time();
    ASSERT_NE( ans = g->pattern_matching(schedule, thread_num), 0);
    t2 = get_wall_time();

    printf("%lld %.6lf\n", ans, t2 - t1);
    
    delete g;
}

TEST(oriented_test, patents_hourglass_oriented_2) {
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

    int dataset_vertex_size = 3072441;
    int dataset_edge_size = 117185083;
    int size = 6;

    Pattern pattern(size);
    pattern.add_edge(0, 1);
    pattern.add_edge(1, 2);
    pattern.add_edge(2, 3);
    pattern.add_edge(0, 3);
    pattern.add_edge(0, 4);
    pattern.add_edge(1, 4);
    pattern.add_edge(2, 5);
    pattern.add_edge(3, 5);
    pattern.add_edge(4, 5);

    bool is_valid;
    Schedule schedule(pattern, is_valid, true, dataset_vertex_size, dataset_edge_size);
    
    std::vector< std::pair<int,int> > pairs;
    int multi = schedule.aggresive_optimize( schedule.get_adj_mat_ptr(), pairs);
    schedule.add_restrict(pairs);

    int thread_num = 24;
    double t1,t2;
    long long ans;
    
    t1 = get_wall_time();
    ASSERT_NE( ans = g->pattern_matching(schedule, thread_num), 0);
    t2 = get_wall_time();

    printf("%lld %.6lf\n", ans, t2 - t1);
    
    delete g;
}

