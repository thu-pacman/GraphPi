#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>

TEST(order_test, livejournal_qg5_unoriented) {
    printf("order_test begin\n");
    Graph *g;
    DataLoader D;
    
    std::string type = "LiveJournal";
    std::string path = "/home/zms/livejournal_input";
    DataType my_type;
    if(type == "LiveJournal") my_type = DataType::LiveJournal;
    else {
        printf("invalid DataType!\n");
    }
    
    ASSERT_EQ(D.load_data(g,my_type,path.c_str()),true); 
    
    printf("Load data success!\n");

    int thread_num = 24;
    double t1,t2;
    std::vector < std::pair<int, int> > pairs;

    int order[5];
    int rank[5];
    for(int i = 0; i < 5; ++i) order[i] = i;
    
    long long ans = -1;
//    do {
        for(int i = 0; i < 5; ++i) rank[order[i]] = i;

        Pattern cur_pattern(5);
        cur_pattern.add_edge(std::min(rank[0],rank[1]), std::max(rank[0],rank[1]));
        cur_pattern.add_edge(std::min(rank[0],rank[4]), std::max(rank[0],rank[4]));
        cur_pattern.add_edge(std::min(rank[1],rank[2]), std::max(rank[1],rank[2]));
        cur_pattern.add_edge(std::min(rank[1],rank[4]), std::max(rank[1],rank[4]));
        cur_pattern.add_edge(std::min(rank[2],rank[3]), std::max(rank[2],rank[3]));
        cur_pattern.add_edge(std::min(rank[3],rank[4]), std::max(rank[3],rank[4]));
        
        bool flag;
        Schedule cur_schedule(cur_pattern, flag);
//        if(flag == false) continue;
        int multiplicity = cur_pattern.aggresive_optimize(pairs);
        cur_schedule.add_restrict(pairs);
        t1 = get_wall_time();
        if ( ans == -1 ) ASSERT_NE( ans = g->pattern_matching(cur_schedule, thread_num), 0);
        else ASSERT_EQ( g->pattern_matching(cur_schedule, thread_num), ans);
        t2 = get_wall_time();
        for(int i = 0; i < 5; ++i)
            printf("%d ",order[i]);
        puts("");
        for(int i = 0; i < pairs.size(); ++i)
            printf("(%d,%d) ", order[pairs[i].first], order[pairs[i].second]);
        puts("");
        printf("%.6lf\n", t2 - t1);

//    } while(std::next_permutation(order, order + 5));

    delete g;
}
