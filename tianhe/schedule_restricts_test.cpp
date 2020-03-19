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

void test_pattern(Graph* g, const Pattern &pattern, int performance_modeling_type, int restricts_type, bool use_in_exclusion_optimize = false) {
    long long tri_cnt = 7515023;
    int thread_num = 24;
    double t1,t2;
    
    bool is_pattern_valid;
    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
    assert(is_pattern_valid);

    if(schedule.get_multiplicity() == 1) return;
    
    t1 = get_wall_time();
    long long ans = g->pattern_matching(schedule, thread_num);
    t2 = get_wall_time();

    printf("ans %lld,%.6lf\n", ans, t2 - t1);
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

    Pattern p(atoi(argv[1]), argv[2]);
    test_pattern(g, p, 1, 1, true);
    test_pattern(g, p, 1, 1);
    test_pattern(g, p, 1, 2);
    test_pattern(g, p, 2, 1);
    test_pattern(g, p, 2, 2);

/*    
    int rank = atoi(argv[1]);

    for(int size = 3; size < 7; ++size) {
        MotifGenerator mg(size);
        std::vector<Pattern> patterns = mg.generate();
        int len = patterns.size();
        for(int i = rank; i < patterns.size(); i += 20) {
            Pattern& p = patterns[i];
            test_pattern(g, p, 1, 1);
            test_pattern(g, p, 1, 2);
            test_pattern(g, p, 2, 1);
            test_pattern(g, p, 2, 2);
        }
    }
*/
/*
    Pattern p(6);
    p.add_edge(0, 1);
    p.add_edge(0, 2);
    p.add_edge(0, 3);
    p.add_edge(1, 4);
    p.add_edge(1, 5);
    test_pattern(g, p, 1, 1);
    test_pattern(g, p, 1, 2);
    test_pattern(g, p, 2, 1);
    test_pattern(g, p, 2, 2);
*/
    delete g;
}
