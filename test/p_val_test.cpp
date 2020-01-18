#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"

#include <iostream>
#include <string>
#include <algorithm>

TEST(p_val_test, p_val) {
    Graph *g;
    DataLoader D;
    
    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }

    std::vector<long long> graph_degree_info;
    std::vector<long long> graph_size_info;

    //pattern is qg5
    int pattern_size = 5;
    int pattern_diameter = 2;
    int max_pattern_degree = 3;

    ASSERT_EQ(D.load_data(g,my_type,path.c_str(), pattern_size, max_pattern_degree, pattern_diameter, graph_degree_info, graph_size_info),true); 
    

    Pattern pattern(pattern_size);
    pattern.add_edge(0, 1);
    pattern.add_edge(0, 4);
    pattern.add_edge(1, 2);
    pattern.add_edge(1, 4);
    pattern.add_edge(2, 3);
    pattern.add_edge(3, 4);

    bool is_valid;
    Schedule schedule(pattern, is_valid, true, graph_degree_info, graph_size_info);
   
    delete g;
}
