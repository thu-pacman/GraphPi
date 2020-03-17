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

void test_pattern(Graph* g, Pattern &pattern, int restricts_type) {
    long long tri_cnt = 7515023;
    int thread_num = 24;
    double t1,t2;
    
    bool is_pattern_valid;
    bool use_in_exclusion_optimize = false;
    
    Schedule schedule(pattern, is_pattern_valid, 0, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
    assert(is_pattern_valid);

    if(schedule.get_multiplicity() == 1) return;
    
    t1 = get_wall_time();
    long long ans = g->pattern_matching(schedule, thread_num);
    t2 = get_wall_time();

    printf("%lld %d %.6lf\n", ans, restricts_type, t2 - t1);

}

void test(Graph* g, PatternType type) {
    Pattern pattern(type);
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
        test_pattern(g, cur_pattern, 1);
        test_pattern(g, cur_pattern, 2);
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

    test(g, PatternType::Rectangle);
//    test(g, PatternType::QG3);
//    test(g, PatternType::Pentagon);
//    test(g, PatternType::House);
//    test(g, PatternType::Hourglass);
//    test(g, PatternType::Cycle_6_Tri);
//    test(g, PatternType::Clique_7_Minus);

    
    delete g;
}
