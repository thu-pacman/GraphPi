#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>

TEST(patentes_qg5_test, patents_qg5) {
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

    int thread_num = 24;
    double t1,t2;
    std::vector < std::pair<int, int> > pairs;

    int order[5];
    int rank[5];
    
    order[0] = 1;
    order[1] = 4;
    order[2] = 2;
    order[3] = 0;
    order[4] = 3;

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
    int multiplicity = cur_pattern.aggresive_optimize(pairs);
    cur_schedule.add_restrict(pairs);
    t1 = get_wall_time();
    long long ans;
    ASSERT_NE(ans = g->pattern_matching(cur_schedule, thread_num), 0);

    printf("%lld\n", ans);
    t2 = get_wall_time();
    printf("%.6lf\n", t2 - t1);

    delete g;
}
