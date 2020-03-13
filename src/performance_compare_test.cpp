#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"
#include "../include/motif_generator.h"

#include <assert.h>
#include <iostream>
#include <string>
#include <algorithm>

void test_pattern(Graph* g, Pattern &pattern) {
    printf("test_pattern\n");
    int thread_num = 24;
    double t1,t2,t3,t4;
    int tri_cnt = 7515023;
    
    bool is_pattern_valid;
    int performance_modeling_type;
    bool use_in_exclusion_optimize;
    
    double t5 = get_wall_time();
    performance_modeling_type = 1;
    use_in_exclusion_optimize = false;
    Schedule schedule_our(pattern, is_pattern_valid, performance_modeling_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
    assert(is_pattern_valid==true);
    double t6 = get_wall_time();

    if(schedule_our.get_multiplicity() == 1) return;

    double t7 = get_wall_time();
    performance_modeling_type = 3;
    use_in_exclusion_optimize = false;
    Schedule schedule_gz(pattern, is_pattern_valid, performance_modeling_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
    assert(is_pattern_valid==true);
    double t8 = get_wall_time();

    std::vector< std::pair<int,int> > gz_pairs;
    schedule_gz.GraphZero_aggressive_optimize(gz_pairs);
    schedule_gz.add_restrict(gz_pairs);

    std::vector< std::vector< std::pair<int,int> > >restricts;
    schedule_our.restricts_generate(schedule_our.get_adj_mat_ptr(), restricts);

    std::vector< std::pair<int,int> > our_pairs;
    schedule_our.restrict_selection(g->v_cnt, g->e_cnt, tri_cnt, restricts, our_pairs);
    schedule_our.add_restrict(our_pairs);
    
    t1 = get_wall_time();
    long long ans_our = g->pattern_matching(schedule_our, thread_num);
    t2 = get_wall_time();

    t3 = get_wall_time();
    long long ans_gz = g->pattern_matching(schedule_gz, thread_num);
    t4 = get_wall_time();

    printf("our ans: %lld perf_time: %.6lf time: %.6lf\n", ans_our, t6 - t5, t2 - t1);
    schedule_our.print_schedule();
    for(int i = 0; i < our_pairs.size(); ++i)
        printf("(%d,%d)",our_pairs[i].first, our_pairs[i].second);
    puts("");
    fflush(stdout);
    printf("GZ  ans: %lld perf_time: %.6lf time: %.6lf\n", ans_gz, t8 - t7, t4 - t3);
    schedule_gz.print_schedule();
    for(int i = 0; i < gz_pairs.size(); ++i)
        printf("(%d,%d)",gz_pairs[i].first, gz_pairs[i].second);
    puts("");
    fflush(stdout);
}

int readBit() {
    char ch = getchar();
    while( ch != '0' && ch != '1') ch = getchar();
    return ch - '0';
}

int main(int argc,char *argv[]) { 
    int pattern_size;
    scanf("%d", &pattern_size);
    Pattern pattern(pattern_size);
    for(int i = 0; i < pattern_size; ++i)
        for(int j = 0; j < pattern_size; ++j) 
            if(readBit() == 1) pattern.add_edge(i,j);

    Graph *g;
    DataLoader D;

    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }

    assert(D.load_data(g,my_type,path.c_str())==true); 

    printf("Load data success!\n");
    fflush(stdout);

    test_pattern(g, pattern);

    //   test_pattern(g, PatternType::Rectangle);
    //   test_pattern(g, PatternType::QG3);
    //   test_pattern(g, PatternType::Pentagon);
    /*    Pattern pattern_house(PatternType::House);
          Pattern pattern_hourglass(PatternType::Hourglass);
          Pattern pattern_cycle_6_tri(PatternType::Cycle_6_Tri);
          Pattern pattern_clique_7_minus(PatternType::Clique_7_Minus);

          test_pattern(g, pattern_house);
          test_pattern(g, pattern_hourglass);
          test_pattern(g, pattern_cycle_6_tri);
          test_pattern(g, pattern_clique_7_minus);
     */
    /*
       for(int size = 6; size < 7; ++size) {
       MotifGenerator mg(size);
       std::vector<Pattern> patterns = mg.generate();
       for(int i = patterns.size() / 3 + 2; i < patterns.size(); ++i) {
       test_pattern(g, patterns[i]);
       }
       }
     */
    /*
       Pattern pattern(7);
       for(int i = 0; i < 3; ++i) {
       pattern.add_edge(i, i * 2 + 1);
       pattern.add_edge(i, i * 2 + 2);
       }

     */
    /*
    // this is pattern273
    Pattern pattern(6);
    pattern.add_edge(0, 1);
    pattern.add_edge(0, 2);
    pattern.add_edge(0, 3);
    pattern.add_edge(0, 4);
    pattern.add_edge(0, 5);
    pattern.add_edge(1, 2);
    pattern.add_edge(2, 3);
    pattern.add_edge(2, 4);
    pattern.add_edge(2, 5);
     */
    /*     
    // this is pattern317
    Pattern pattern(6);
    pattern.add_edge(0, 3);
    pattern.add_edge(0, 4);
    pattern.add_edge(0, 5);
    pattern.add_edge(1, 3);
    pattern.add_edge(1, 4);
    pattern.add_edge(1, 5);
    pattern.add_edge(2, 3);
    pattern.add_edge(2, 4);
    pattern.add_edge(2, 5);
    pattern.add_edge(0, 1);
    pattern.add_edge(0, 2);
    test_pattern(g, pattern);
*/     
    delete g;
}
