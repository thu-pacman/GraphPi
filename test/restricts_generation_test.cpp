#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>
#include <algorithm>

TEST(restricts_generation_test, restricts_generation_pattern3x3) {
    Graph *g;
    DataLoader D;
    
    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }

    std::vector<long long> graph_degree_info;
    std::vector<long long> graph_size_info;
    
    int pattern_size = 6;
    int pattern_diameter = 1;
    int max_pattern_degree = 2;
    
    Pattern pattern(pattern_size);
    pattern.add_edge(0, 1);
    pattern.add_edge(0, 2);
    pattern.add_edge(0, 3);
    pattern.add_edge(4, 1);
    pattern.add_edge(4, 2);
    pattern.add_edge(4, 3);
    pattern.add_edge(5, 1);
    pattern.add_edge(5, 2);
    pattern.add_edge(5, 3);
    
    ASSERT_EQ(D.load_data(g,my_type,path.c_str(), pattern_size, max_pattern_degree, pattern_diameter, graph_degree_info, graph_size_info),true); 
    
    printf("Load data success!\n");
    fflush(stdout);

    bool is_pattern_valid;
    bool use_performance_modeling = false;
    Schedule schedule(pattern, is_pattern_valid, use_performance_modeling, graph_degree_info, graph_size_info);

    ASSERT_EQ(is_pattern_valid, true);

    std::vector< std::vector< std::pair<int,int> > >pairs_group;
    schedule.aggressive_optimize_get_all_pairs( schedule.get_adj_mat_ptr(), pairs_group);
    
    std::vector< std::pair<int,int> > gz_pairs;
    schedule.GraphZero_aggressive_optimize( schedule.get_adj_mat_ptr(), gz_pairs);

    schedule.add_restrict(gz_pairs);

    int thread_num = 24;
    double t1,t2;
    long long ans_gz;
    
    t1 = get_wall_time();
    ans_gz = g->pattern_matching(schedule, thread_num);
    t2 = get_wall_time();

    printf("GraphZero's ans: %lld time: %.6lf\n restricts: ", ans_gz, t2 - t1);
    for(int i = 0; i < gz_pairs.size(); ++i)
        printf("(%d,%d)",gz_pairs[i].first, gz_pairs[i].second);
    puts("");
    fflush(stdout);

    for(int rank = 0; rank < pairs_group.size(); ++rank) {
        bool is_pattern_valid;
        Schedule schedule(pattern, is_pattern_valid, use_performance_modeling, graph_degree_info, graph_size_info);

        ASSERT_EQ(is_pattern_valid, true);

        schedule.add_restrict(pairs_group[rank]);

        int thread_num = 24;
        double t1,t2;
        long long ans;

        t1 = get_wall_time();
        ans = g->pattern_matching(schedule, thread_num);
        t2 = get_wall_time();

        if( ans != ans_gz) continue;
        printf("Ours' ans: %lld time: %.6lf\n restricts: ", ans, t2 - t1);
        for(int i = 0; i < pairs_group[rank].size(); ++i)
            printf("(%d,%d)", pairs_group[rank][i].first, pairs_group[rank][i].second);
        puts("");
        fflush(stdout);

    }

    delete g;
}
