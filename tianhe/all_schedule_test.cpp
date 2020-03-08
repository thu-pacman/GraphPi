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
    double t1,t2;
    
    bool is_pattern_valid;
    int performance_modeling_type;
    bool use_in_exclusion_optimize;

    int size = pattern.get_size();
    const int* adj_mat = pattern.get_adj_mat_ptr();
    int rank[size];
    for(int i = 0; i < size; ++i) rank[i] = i;
    do{
        Pattern cur_pattern(size);
        for(int i = 0; i < size; ++i)
            for(int j = 0; j < i; ++j)
                if( adj_mat[INDEX(i,j,size)])
                    cur_pattern.add_edge(rank[i],rank[j]);

        performance_modeling_type = 0;
        use_in_exclusion_optimize = false;
        Schedule schedule(cur_pattern, is_pattern_valid, performance_modeling_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt);
        if(is_pattern_valid == false) continue;

        std::vector< std::pair<int,int> > gz_pairs;
        schedule.GraphZero_aggressive_optimize(gz_pairs);
        schedule.add_restrict(gz_pairs);

        t1 = get_wall_time();
        long long ans = g->pattern_matching(schedule, thread_num);
        t2 = get_wall_time();
        for(int i = 0; i < size; ++i)
            printf("%d,",rank[i]);
        printf("%lld,%.6lf\n", ans, t2 - t1);
        fflush(stdout); 
    } while( std::next_permutation(rank, rank + size));

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

    Pattern pattern(PatternType::House);
    test_pattern(g, pattern);

    delete g;
}
