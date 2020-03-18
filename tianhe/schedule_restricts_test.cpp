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

void test_pattern(Graph* g, Pattern &pattern, int performance_modeling_type, int restricts_type) {
    long long tri_cnt = 7515023;
    int thread_num = 24;
    double t1,t2;
    
    bool is_pattern_valid;
    bool use_in_exclusion_optimize = false;
    
    double t3 = get_wall_time();
    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
    assert(is_pattern_valid);

    if(schedule.get_multiplicity() == 1) return;

    double t4 = get_wall_time();
    
    t1 = get_wall_time();
    long long ans = g->pattern_matching(schedule, thread_num);
    t2 = get_wall_time();

    printf("ans %lld, %.6lf,%.6lf\n", ans,t4 - t3, t2 - t1);
    schedule.print_schedule();
    const auto& pairs = schedule.restrict_pair;
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

    Pattern p(PatternType::Cycle_6_Tri);
    test_pattern(g, p, 1, 1);
    test_pattern(g, p, 1, 2);
    test_pattern(g, p, 2, 1);
    test_pattern(g, p, 2, 2);

    /*
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
       test_pattern(g, p, 1, 1);
       test_pattern(g, p, 1, 2);
       test_pattern(g, p, 2, 1);
       test_pattern(g, p, 2, 2);
       }
       }
     */
    delete g;
}
