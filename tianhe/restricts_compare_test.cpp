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
    long long tri_cnt = 7515023;
    int thread_num = 24;
    double t1,t2,t3,t4;
    
    bool is_pattern_valid;
    bool use_in_exclusion_optimize = false;
    
    Schedule schedule_our(pattern, is_pattern_valid, 0, 1, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
    assert(is_pattern_valid);
    
    Schedule schedule_gz(pattern, is_pattern_valid, 0, 2, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
    assert(is_pattern_valid);

    if(schedule_our.get_multiplicity() == 1) return;
    auto pairs_our = schedule_our.restrict_pair;
    auto pairs_gz  = schedule_gz.restrict_pair;
    schedule_our.print_schedule();
    printf("%d ", pairs_our.size());
    for(const auto& p : pairs_our)
        printf("(%d,%d)", p.first, p.second);
    puts("");
    printf("%d ", pairs_gz.size());
    for(const auto& p : pairs_gz)
        printf("(%d,%d)", p.first, p.second);
    puts("");
    fflush(stdout); 
    if(pairs_our.size() == pairs_gz.size()) {
        std::sort(pairs_our.begin(), pairs_our.end());
        std::sort(pairs_gz.begin(), pairs_gz.end());
        bool is_same = true;
        for(unsigned int i = 0; i < pairs_our.size(); ++i)
            if(pairs_our[i] != pairs_gz[i]) {
                is_same = false;
                break;
            }
        if(is_same) return;
    }
    
    t1 = get_wall_time();
    long long ans1 = g->pattern_matching(schedule_our, thread_num);
    t2 = get_wall_time();
    
    t3 = get_wall_time();
    long long ans2 = g->pattern_matching(schedule_gz, thread_num);
    t4 = get_wall_time();

    printf("%lld %lld %.6lf %.6lf\n", ans1, ans2, t2 - t1, t4 - t3);
    schedule_our.print_schedule();
    printf("%d ", pairs_our.size());
    for(const auto& p : pairs_our)
        printf("(%d,%d)", p.first, p.second);
    puts("");
    printf("%d ", pairs_gz.size());
    for(const auto& p : pairs_gz)
        printf("(%d,%d)", p.first, p.second);
    puts("");
    fflush(stdout); 
}

void test(Graph* g, Pattern pattern) {
    const int* adj_mat = pattern.get_adj_mat_ptr();
    int size = pattern.get_size();
    int vec[size];
    for(int i = 0; i < size; ++i) vec[i] = i;
    do {
        bool connected = true;
        for(int i = 1; i < size; ++i) {
            bool have_edge = false;
            for(int j = 0; j < i; ++j)
                if( adj_mat[INDEX(vec[i],vec[j],size)]) {
                    have_edge = true;
                    break;
                }
            if(!have_edge) {
                connected = false;
                break;
            }
        }
        if(!connected) continue;

        Pattern cur_pattern(size);
        for(int i = 0; i < size; ++i)
            for(int j = 0; j < size; ++j)
                if(adj_mat[INDEX(vec[i], vec[j], size)])
                    cur_pattern.add_edge(i,j);
        test_pattern(g, cur_pattern);
    } while( std::next_permutation( vec, vec + size));
}

int main(int argc,char *argv[]) {
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
/*
    int rank = atoi(argv[1]);

    for(int size = 3; size < 7; ++size) {
        MotifGenerator mg(size);
        std::vector<Pattern> patterns = mg.generate();
        int len = patterns.size();
        for(int i = rank; i < patterns.size(); i += 20) {
            Pattern& p = patterns[i];
            test(g, p);
        }
    }
*/
    Pattern p_qg3(PatternType::QG3);
    Pattern p_rec(PatternType::Rectangle);
    Pattern p_pen(PatternType::Pentagon);
    Pattern p_house(PatternType::House);
    Pattern p_hour(PatternType::Hourglass);
    Pattern p_cyc(PatternType::Cycle_6_Tri);
    Pattern p_c7m(PatternType::Clique_7_Minus);

       // test(g, p_qg3);
     //   test(g, p_rec);
   //     test(g, p_pen);
        test(g, p_house);
     //   test(g, p_hour);
     //   test(g, p_cyc);
     //   test(g, p_c7m);


    delete g;
}
