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

void test_pattern(Graph* g, const Pattern &pattern, bool IEP_FLAG) {
    int thread_num = 24;
    double t1,t2;
    
    bool is_pattern_valid;
    Schedule schedule(pattern, is_pattern_valid, 1, 1, IEP_FLAG, g->v_cnt, g->e_cnt, g->tri_cnt);
    assert(is_pattern_valid);

    std::cout << "Counting...Please wait." << std::endl;
    t1 = get_wall_time();
    long long ans = g->pattern_matching(schedule, thread_num);
    t2 = get_wall_time();
    
    printf("Counting result is %lld.(times of pattern appear in the dataset)\n", ans);

    printf("Pattern matching use %.6lf seconds.\n", t2 - t1);

    printf("Searching details: \n");
    schedule.print_schedule();
    printf("Restricts :\n");
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

    std::string path;
    long long tri_cnt;

    std::cout << "Please input the path of dataset:";
    std::cin >> path;

    std::cout << "Please input the number of triangles in the dataset:";
    std::cin >> tri_cnt;

    int size;

    std::cout << "Please input the number of nodes in the pattern:";
    std::cin >> size;

    int m;
    std::cout << "Please input the number of edges in the pattern:";
    std::cin >> m;

    Pattern p(size);

    std::cout << "Please input the edges in the pattern:" << std::endl;
    for(int i = 0; i < m; ++i) {
        int x,y;
        std::cin >> x >> y;
        p.add_edge(x,y);
        p.add_edge(y,x);
    }
    
    std::string flag;
    std::cout << "Do you want to use inclusion-exclusion optimization?[y/n]";
    std::cin >> flag;

    bool IEP_FLAG;
    if(flag == "y") {
        IEP_FLAG = true;
        std::cout << "GraphPi will enable inclusion-exclusion optimization." << std::endl;
    }
    else {
        IEP_FLAG = false;
        std::cout << "GraphPi will disable inclusion-exclusion optimization." << std::endl;
    }
    
    std::cout << "Loading dataset from file." << std::endl;
    double t1 = get_wall_time();
    assert(D.load_data(g,tri_cnt,path.c_str())==true); 
    double t2 = get_wall_time();
    printf("Load data success!\n");
    printf("Load data usage: %.6lf seconds\n", t2 - t1);

    fflush(stdout);

    test_pattern(g, p, IEP_FLAG);
    
    delete g;
}
