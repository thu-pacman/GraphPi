#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>
#include <algorithm>

TEST(performance_test, performance_orkut_qg3) {
    Graph *g;
    DataLoader D;
    
    std::string type = "Orkut";
    std::string path = "/home/zms/orkut_input";
    DataType my_type;
    if(type == "Orkut") my_type = DataType::Orkut;
    else {
        printf("invalid DataType!\n");
    }
    
    ASSERT_EQ(D.load_data(g,my_type,path.c_str()),true); 
    
    printf("Load data success!\n");
    int dataset_vertex_size = 3072441;
    int dataset_edge_size = 117185083;
    int size = 4;

    Pattern pattern(size);
    pattern.add_edge(0, 1);
    pattern.add_edge(0, 2);
    pattern.add_edge(0, 3);
    pattern.add_edge(1, 2);
    pattern.add_edge(2, 3);

    bool is_valid;
    Schedule schedule(pattern, is_valid, true, dataset_vertex_size, dataset_edge_size);
    
    std::vector< std::pair<int,int> > pairs;
    int multi = schedule.aggresive_optimize( schedule.get_adj_mat_ptr(), pairs);
    schedule.add_restrict(pairs);

    const int *adj_mat = schedule.get_adj_mat_ptr();
    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j)
            printf("%d ",adj_mat[INDEX(i, j, size)]);
            puts("");
    }
    for(int i = 0; i < pairs.size(); ++i)
        printf("(%d,%d) ", pairs[i].first, pairs[i].second);
    puts("");

    int thread_num = 24;
    double t1,t2;
    long long ans;
    
    t1 = get_wall_time();
    ASSERT_NE( ans = g->pattern_matching(schedule, thread_num), 0);
    t2 = get_wall_time();

    printf("%lld %.6lf\n", ans, t2 - t1);

    int order[size];
    for(int i = 0; i < size; ++i) order[i] = i;
    do{
        Pattern pattern(size);
        int rank[size];
        for(int i = 0; i < size; ++i) rank[order[i]] = i;
        pattern.add_edge(std::min(rank[0], rank[1]), std::max(rank[0], rank[1])); 
        pattern.add_edge(std::min(rank[0], rank[2]), std::max(rank[0], rank[2])); 
        pattern.add_edge(std::min(rank[0], rank[3]), std::max(rank[0], rank[3])); 
        pattern.add_edge(std::min(rank[1], rank[2]), std::max(rank[1], rank[2])); 
        pattern.add_edge(std::min(rank[2], rank[3]), std::max(rank[2], rank[3])); 
        
        Schedule schedule(pattern, is_valid, false, 0, 0);
        if( !is_valid) continue;

        std::vector< std::pair<int,int> > pairs;
        int multi = schedule.aggresive_optimize( schedule.get_adj_mat_ptr(), pairs);
        schedule.add_restrict(pairs);
        
        t1 = get_wall_time();
        ASSERT_EQ( g->pattern_matching(schedule, thread_num), ans);
        t2 = get_wall_time();

        for(int i = 0; i < size; ++i) printf("%d ", order[i]);
        printf("%.6lf\n", t2 - t1);

    } while(std::next_permutation(order, order + size));
    
}
