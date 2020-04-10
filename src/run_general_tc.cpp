#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"


#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc,char *argv[])
{
    if (argc != 3)
    {
        printf("Please give a name of dataset, like Patents, and a thread number.");
        return -1;
    }
    Graph *g;
    DataLoader D;
    
    std::string type = std::string(argv[1]);
    std::string path;
    DataType my_type;
    if (type == "Patents")
    {
        path = "/home/zms/patents_input";
        my_type = DataType::Patents;
    }
    else if (type == "Orkut")
    {
        path = "/home/zms/orkut_input";
        my_type = DataType::Orkut;
    }
    else {
        printf("Invalid dataset!\n");
        return -1;
    }
    
    if (D.load_data(g,my_type,path.c_str()) != true)
    {
        printf("Load error.");
        return -1;
    }

    Pattern tc_pattern(3);
    tc_pattern.add_edge(0, 1);
    tc_pattern.add_edge(0, 2);
    tc_pattern.add_edge(1, 2);
    bool is_pattern_valid;
    bool use_in_exclusion_optimize = false;
    int performance_type = 0;
    int restricts_type = 0;
    Schedule tc_schedule(tc_pattern, is_pattern_valid, performance_type, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt);

    int thread_num = atoi(argv[2]);
    double t1 = get_wall_time();
    g->pattern_matching(tc_schedule, thread_num, true);
    double t2 = get_wall_time();
    printf("brute force %d thread TC with root symmetry time: %.6lf\n", thread_num, t2 - t1);
}
