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
    pattern.print();
    int thread_num = 24;
    long long tri_cnt = 7515023;
    double t1,t2;
    
    bool is_pattern_valid;
    int performance_modeling_type = 3;
    bool use_in_exclusion_optimize = false;
    
    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
    assert(is_pattern_valid);
    
    std::vector< std::vector< std::pair<int,int> > >restricts;
    schedule.restricts_generate(schedule.get_adj_mat_ptr(), restricts);
    
    schedule.print_schedule();

    for( auto& pairs : restricts) {
        Schedule cur_schedule(pattern, is_pattern_valid, performance_modeling_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, tri_cnt);
        cur_schedule.add_restrict(pairs);
        double t1 = get_wall_time();
        long long ans = g->pattern_matching(cur_schedule, thread_num);
        double t2 = get_wall_time();
        printf("ans %lld time %.6lf\n", ans, t2 - t1);
        printf("%d ", pairs.size());
        for(const auto& p : pairs)
            printf("(%d,%d)", p.first, p.second);
        puts("");
    }
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
            test_pattern(g, p);
        }
    }

    delete g;
}
