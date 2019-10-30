#include <gtest/gtest.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/motif_generator.h"
#include "../include/dataloader.h"
#include "../include/graph.h"

#include <iostream>
#include <string>
#include <vector>

TEST(schedule_test, build_loop_invariant)
{
    Pattern tc_pattern(3);
    tc_pattern.add_edge(0, 1);
    tc_pattern.add_edge(0, 2);
    tc_pattern.add_edge(1, 2);
    Schedule tc_schedule(tc_pattern);
    ASSERT_EQ(tc_schedule.get_total_prefix_num(), 2);
    ASSERT_EQ(tc_schedule.get_last(0), 0);
    ASSERT_EQ(tc_schedule.get_last(1), 1);
    ASSERT_EQ(tc_schedule.get_loop_set_prefix_id(1), 0);
    ASSERT_EQ(tc_schedule.get_loop_set_prefix_id(2), 1);

    Pattern pattern5(5);
    pattern5.add_edge(0, 1);
    pattern5.add_edge(1, 2);
    for (int i = 3; i < 5; ++i)
        for (int j = 0; j < 3; ++j)
            pattern5.add_edge(i, j);
    Schedule schedule5(pattern5);
    //Prefix:
    //0: N(0)
    //1: N(1)
    //2: N(0) N(1)
    //3: N(0) N(1) N(2)
    ASSERT_EQ(schedule5.get_total_prefix_num(), 4);
    ASSERT_EQ(schedule5.get_last(0), 0);
    ASSERT_EQ(schedule5.get_last(1), 2);
    ASSERT_EQ(schedule5.get_last(2), 3);
    ASSERT_EQ(schedule5.get_loop_set_prefix_id(1), 0);
    ASSERT_EQ(schedule5.get_loop_set_prefix_id(2), 1);
    ASSERT_EQ(schedule5.get_loop_set_prefix_id(3), 3);
    ASSERT_EQ(schedule5.get_loop_set_prefix_id(4), 3);
    ASSERT_EQ(schedule5.get_next(2), 1);
    ASSERT_EQ(schedule5.get_father_prefix_id(3), 2);
    ASSERT_EQ(schedule5.get_father_prefix_id(2), 0);

}

TEST(motif_generator_test, motif_generator)
{
    MotifGenerator mg0(3);
    std::vector<Pattern> patterns = mg0.generate();
    ASSERT_EQ(patterns.size(), 2);
    for (const Pattern& p : patterns)
    {
        p.print();
    }
    MotifGenerator mg1(4);
    patterns = mg1.generate();
    ASSERT_EQ(patterns.size(), 6);
}

TEST(aggressive_optimize_test, aggre_ssive_optimize)
{

    Graph *g;
    DataLoader D;
    
    std::string type = "complete8";
    std::string path = "/home/sth/download/8nodes_complete_graph.txt";
    DataType my_type;
    my_type = DataType::complete8;
    
    ASSERT_EQ(D.load_data(g,my_type,path.c_str()),true); 

    std::vector<Pattern> patterns;
    for (int i = 3; i <= 7; ++i)//more than 6 will cost too much time
    {
        MotifGenerator mg(i);
        patterns = mg.generate();
        std::vector < std::pair<int, int> > pairs;
        for (const Pattern& p : patterns)
        {
            int multiplicity = p.aggresive_optimize(pairs); // check if the isomorphism_vec size can be deleted to 1 by restriction.("assert(isomorphism_vec.size() == 1);")
            Schedule schedule(p);
            int thread_count = 24;
            long long ans_conservative = g->pattern_matching(schedule, thread_count);
            schedule.add_restrict(pairs);
            long long ans_aggressive = g->pattern_matching(schedule, thread_count);
            ASSERT_EQ(ans_conservative, ans_aggressive * multiplicity);
            ASSERT_NE(ans_conservative, 0);
        }
    }

    /*Pattern p(4);
    p.add_edge(0, 1);
    p.add_edge(0, 2);
    p.add_edge(2, 3);
    p.add_edge(1, 3);
    std::vector < std::pair<int, int> > pairs;
    int multiplicity = p.aggresive_optimize(pairs);
    Schedule schedule(p);
    int thread_count = 24;
    int ans_conservative = g->pattern_matching(schedule, thread_count);
    schedule.add_restrict(pairs);
    int ans_aggressive = g->pattern_matching(schedule, thread_count);
    ASSERT_EQ(ans_conservative, ans_aggressive * multiplicity);*/
}