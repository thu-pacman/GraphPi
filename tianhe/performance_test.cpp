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

void test_pattern(Graph* g, const Pattern &pattern, std::vector< std::pair<int,int> > &our_pairs) {
    int thread_num = 24;
    double t1,t2;
    
    bool is_pattern_valid;
    Schedule schedule(pattern, is_pattern_valid, 0,0,0, g->v_cnt, g->e_cnt, g->tri_cnt);
    assert(is_pattern_valid);
    schedule.add_restrict(our_pairs);

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

    const std::string type = argv[1];
    const std::string path = argv[2];
    
    int size = atoi(argv[3]);
    char* adj_mat = argv[4];

    int restricts_size = atoi(argv[5]);
    std::vector< std::pair<int,int> > pairs;
    pairs.clear();
    for(int i = 0; i < restricts_size; ++i) {
        int x = atoi(argv[6 + i * 2]);
        int y = atoi(argv[6 + i * 2 + 1]);
        pairs.push_back(std::make_pair(x,y));
    }

    DataType my_type;
    
    GetDataType(my_type, type);

    if(my_type == DataType::Invalid) {
        printf("Dataset not found!\n");
        return 0;
    }

    double t1 = get_wall_time();
    assert(D.load_data(g,my_type,path.c_str())==true); 
    double t2 = get_wall_time();
    printf("Load time %.6lf\n", t2 - t1);

    printf("Load data success!\n");
    fflush(stdout);

    Pattern p(size, adj_mat);
    test_pattern(g, p, pairs);
    
    delete g;
}
