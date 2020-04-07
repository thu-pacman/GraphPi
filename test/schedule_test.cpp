#include <gtest/gtest.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/motif_generator.h"
#include "../include/dataloader.h"
#include "../include/graph.h"

#include <iostream>
#include <string>
#include <vector>
#include <sys/time.h>
#include <cstdlib>

TEST(schedule_test, build_loop_invariant)
{
    /*Pattern tc_pattern(3);
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
    ASSERT_EQ(schedule5.get_father_prefix_id(2), 0);*///TODO:bx2k

}

TEST(motif_generator_test, motif_generator)
{
    /*MotifGenerator mg0(3);
    std::vector<Pattern> patterns = mg0.generate();
    ASSERT_EQ(patterns.size(), 2);
    for (const Pattern& p : patterns)
    {
        p.print();
    }
    MotifGenerator mg1(4);
    patterns = mg1.generate();
    ASSERT_EQ(patterns.size(), 6);*///TODO:bx2k
}

TEST(aggressive_optimize_test, aggre_ssive_optimize)
{
    auto get_wall_time = []() {
        struct timeval time;
        if(gettimeofday(&time,NULL)) {
            return 0.0;
        }
        return (double)((double)time.tv_sec + (double)time.tv_usec * 0.000001);
    };

    Graph *g;
    DataLoader D;
    
    std::string type = "Patents";
    //std::string path = "/home/zms/orkut_input";
    std::string path = "/home/zms/patents_input";
    //std::string path = "/home/xuyi/input/4.in";
    
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }
    
    ASSERT_EQ(D.load_data(g,my_type,path.c_str()),true); 
    MotifGenerator mg(3);
    /*Pattern p(6);
    p.add_edge(0, 1);
    p.add_edge(0, 2);
    p.add_edge(0, 3);
    p.add_edge(0, 5);
    p.add_edge(1, 2);
    p.add_edge(1, 3);
    p.add_edge(1, 4);
    p.add_edge(2, 4);
    p.add_edge(2, 5);*/
    /*Pattern p(4);
    p.add_edge(0, 1);
    p.add_edge(0, 3);
    p.add_edge(1, 2);
    p.add_edge(2, 3);*/
    //p.add_edge(2, 3);
    /*p.add_edge(2, 3);
    p.add_edge(3, 4);
    p.add_edge(4, 1);*/
    Pattern p(5);
    p.add_edge(0, 1);
    p.add_edge(0, 2);
    p.add_edge(0, 3);
    p.add_edge(1, 3);
    p.add_edge(1, 4);
    p.add_edge(2, 4);
    std::vector < std::pair<int, int> > pairs;
    bool is_pattern_valid;
    bool use_in_exclusion_optimize = false;
    Schedule schedule(p, is_pattern_valid, 0, use_in_exclusion_optimize, g->v_cnt, g->e_cnt);
    int thread_count = 24;
    schedule.aggressive_optimize(pairs); // check if the isomorphism_vec size can be deleted to 1 by restriction.("assert(isomorphism_vec.size() == 1);")
    schedule.add_restrict(pairs);
    /*{
        double t1 = get_wall_time();
        //long long ans_aggressive = g->pattern_matching_mpi(schedule, thread_count);
        long long ans_aggressive = g->pattern_matching(schedule, thread_count);
        //ASSERT_EQ(ans_aggressive, 19186236035);
        //ASSERT_EQ(ans_aggressive, 7515023);
        //ASSERT_EQ(ans_aggressive, 627584181);
        //ASSERT_EQ(ans_aggressive, 67098889426);
        double t2 = get_wall_time();
        printf("single node TC time: %.6lf, ans = %lld, expected = 67098889426\n", t2 - t1, ans_aggressive);
        fflush(stdout);
    }*/
    /*{
        double t1 = get_wall_time();
        //long long ans_aggressive = g->pattern_matching_mpi(schedule, thread_count);
        long long ans_aggressive = g->pattern_matching(schedule, thread_count);
        //ASSERT_EQ(ans_aggressive, 19186236035);
        //ASSERT_EQ(ans_aggressive, 7515023);
        //ASSERT_EQ(ans_aggressive, 627584181);
        double t2 = get_wall_time();
        printf("single node TC time: %.6lf, ans = %lld\n", t2 - t1, ans_aggressive);
        fflush(stdout);
    }*/
    {
        double t1 = get_wall_time();
        //long long ans_aggressive = g->pattern_matching_mpi(schedule, thread_count);
        long long ans_aggressive = g->pattern_matching_mpi(schedule, thread_count);
        //ASSERT_EQ(ans_aggressive, 19186236035);
        //ASSERT_EQ(ans_aggressive, 7515023);
        //ASSERT_EQ(ans_aggressive, 627584181);
        double t2 = get_wall_time();
        printf("multi node TC time: %.6lf, ans = %lld\n", t2 - t1, ans_aggressive);
        fflush(stdout);
    }


    /*std::vector<Pattern> patterns;
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
    }*/

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
