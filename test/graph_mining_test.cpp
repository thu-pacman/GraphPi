#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"


#include <iostream>
#include <string>
#include <mpi.h>

TEST(graph_mining_test, patents_triangle_counting) {
    /*int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("%d of %d\n", rank, size);*/
    /*Graph *g;
    DataLoader D;
    
    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    //std::string path = "/home/xuyi/input/4.in";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }
    
    ASSERT_EQ(D.load_data(g,my_type,path.c_str()),true); 

    double t1 = get_wall_time();
    ASSERT_EQ(g->triangle_counting(), 7515023);
    double t2 = get_wall_time();
    printf("brute force single thread TC time: %.6lf\n", t2 - t1);

    double t1 = get_wall_time();
    ASSERT_EQ(g->triangle_counting_mpi(24), 7515023);
    //ASSERT_EQ(g->triangle_counting_mpi(24), 4);
    double t2 = get_wall_time();
    printf("brute force multi thread multi processing TC with mpi time: %.6lf\n", t2 - t1);

    double sum_time = 0.0;
    int thread_num = 24;
    for (int times = 0; times < 10; ++times)
    {
        t1 = get_wall_time();
        ASSERT_EQ(g->triangle_counting_mt(thread_num), 7515023);
        t2 = get_wall_time();
        sum_time += t2 - t1;
    }
    printf("brute force %d thread TC time: %.6lf\n", thread_num, sum_time / 10);

    Pattern tc_pattern(3);
    tc_pattern.add_edge(0, 1);
    tc_pattern.add_edge(0, 2);
    tc_pattern.add_edge(1, 2);
    Schedule tc_schedule(tc_pattern);

    t1 = get_wall_time();
    ASSERT_EQ(g->pattern_matching(tc_schedule, 1), 7515023 * 6);
    t2 = get_wall_time();
    printf("general single thread TC time: %.6lf\n", t2 - t1);

    thread_num = 24;
    t1 = get_wall_time();
    ASSERT_EQ(g->pattern_matching(tc_schedule, thread_num), 7515023 * 6);
    t2 = get_wall_time();
    printf("general %d thread TC without root symmetry time: %.6lf\n", thread_num, t2 - t1);

    t1 = get_wall_time();
    ASSERT_EQ(g->pattern_matching(tc_schedule, thread_num, true), 7515023);
    t2 = get_wall_time();
    printf("general %d thread TC with root symmetry time: %.6lf\n", thread_num, t2 - t1);

    Pattern clique4(4);
    for (int i = 0; i < 4; ++i)
        for (int j = i + 1; j < 4; ++j)
            clique4.add_edge(i, j);
    Schedule clique4_schedule(clique4);

    t1 = get_wall_time();
    //ASSERT_EQ(g->pattern_matching(clique4_schedule, thread_num, true), 7515023);
    g->pattern_matching(clique4_schedule, thread_num, false);
    t2 = get_wall_time();
    printf("general %d thread Clique4 without root symmetry time: %.6lf\n", thread_num, t2 - t1);
    delete g;*/
}

TEST(graph_mining_test, orkut_triangle_counting) {
    /*Graph *g;
    DataLoader D;
    
    std::string type = "Orkut";
    std::string path = "/home/zms/orkut_input";
    DataType my_type;
    my_type = DataType::Orkut;
    
    ASSERT_EQ(D.load_data(g,my_type,path.c_str()),true); 

    double t1 = get_wall_time();
    int thread_num = 24;
    ASSERT_EQ(g->triangle_counting_mt(thread_num), 627584181);
    double t2 = get_wall_time();
    printf("brute force %d thread TC time: %.6lf\n", thread_num, t2 - t1);

    Pattern tc_pattern(3);
    tc_pattern.add_edge(0, 1);
    tc_pattern.add_edge(0, 2);
    tc_pattern.add_edge(1, 2);
    Schedule tc_schedule(tc_pattern);

    thread_num = 24;
    t1 = get_wall_time();
    ASSERT_EQ(g->pattern_matching(tc_schedule, thread_num), 627584181ll * 6);
    t2 = get_wall_time();
    printf("general %d thread TC without root symmetry time: %.6lf\n", thread_num, t2 - t1);

    t1 = get_wall_time();
    ASSERT_EQ(g->pattern_matching(tc_schedule, thread_num, true), 627584181);
    t2 = get_wall_time();
    printf("general %d thread TC with root symmetry time: %.6lf\n", thread_num, t2 - t1);

    Pattern clique4(4);
    for (int i = 0; i < 4; ++i)
        for (int j = i + 1; j < 4; ++j)
            clique4.add_edge(i, j);
    Schedule clique4_schedule(clique4);

    t1 = get_wall_time();
    //ASSERT_EQ(g->pattern_matching(clique4_schedule, thread_num, true), 7515023);
    g->pattern_matching(clique4_schedule, thread_num, false);
    t2 = get_wall_time();
    printf("general %d thread Clique4 without root symmetry time: %.6lf\n", thread_num, t2 - t1);

    Pattern clique6(6);
    for (int i = 0; i < 6; ++i)
        for (int j = i + 1; j < 6; ++j)
            clique6.add_edge(i, j);
    Schedule clique6_schedule(clique6);

    t1 = get_wall_time();
    g->pattern_matching(clique6_schedule, thread_num, true);
    t2 = get_wall_time();
    printf("general %d thread Clique6 with root symmetry time: %.6lf\n", thread_num, t2 - t1);

    Pattern clique7(7);
    for (int i = 0; i < 7; ++i)
        for (int j = i + 1; j < 7; ++j)
            clique7.add_edge(i, j);
    Schedule clique7_schedule(clique7);

    t1 = get_wall_time();
    g->pattern_matching(clique7_schedule, thread_num, true);
    t2 = get_wall_time();
    printf("general %d thread Clique7 with root symmetry time: %.6lf\n", thread_num, t2 - t1);

    Pattern clique8(8);
    for (int i = 0; i < 8; ++i)
        for (int j = i + 1; j < 8; ++j)
            clique8.add_edge(i, j);
    Schedule clique8_schedule(clique8);

    t1 = get_wall_time();
    g->pattern_matching(clique8_schedule, thread_num, true);
    t2 = get_wall_time();
    printf("general %d thread Clique8 with root symmetry time: %.6lf\n", thread_num, t2 - t1);


    delete g;*/
}
