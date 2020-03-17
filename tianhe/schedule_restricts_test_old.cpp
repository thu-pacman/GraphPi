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

void test_pattern(Graph* g, Pattern &pattern, int performance_modeling_type, bool use_our_restricts) {
    int tri_cnt = 7515023;
    int thread_num = 24;
    double t1,t2;
    
    bool is_pattern_valid;
    bool use_in_exclusion_optimize = false;
    
    double t3 = get_wall_time();
    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
    assert(is_pattern_valid);

    if(schedule.get_multiplicity() == 1) return;

    std::vector< std::pair<int,int> > pairs;
    if(use_our_restricts) {
        std::vector< std::vector< std::pair<int,int> > >restricts;
        schedule.restricts_generate(schedule.get_adj_mat_ptr(), restricts);

        schedule.restrict_selection(g->v_cnt, g->e_cnt, tri_cnt, restricts, pairs);
        schedule.add_restrict(pairs);
    }
    else {
        schedule.GraphZero_aggressive_optimize(pairs);
        schedule.add_restrict(pairs);
    }
    double t4 = get_wall_time();
    
    t1 = get_wall_time();
    long long ans = g->pattern_matching(schedule, thread_num);
    t2 = get_wall_time();

    printf("ans %lld, %.6lf,%.6lf\n", ans,t4 - t3, t2 - t1);
    schedule.print_schedule();
    printf("%d ",pairs.size());
    for(auto& p : pairs)
        printf("(%d,%d)",p.first,p.second);
    puts("");
    fflush(stdout);

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


    int rank = 0;

    for(int size = 3; size < 7; ++size) {
        MotifGenerator mg(size);
        std::vector<Pattern> patterns = mg.generate();
        int len = patterns.size();
        int l = len / 10 * rank;
        int r = len / 10 * (rank + 1);
        if( rank == 9) r = len;
        for(int i = l; i < r; ++i) {
            Pattern& p = patterns[i];
            printf("%d\n", size);
            fflush(stdout);
            test_pattern(g, p, 1, false);
            test_pattern(g, p, 1, true);
            test_pattern(g, p, 3, false);
            test_pattern(g, p, 3, true);
        }
    }

    delete g;
}
