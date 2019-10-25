#include <gtest/gtest.h>
#include <../include/graph.h>
#include <../include/dataloader.h>

#include <iostream>
#include <string>

TEST(graph_mining_test, test_triangle_counting) {
    Graph *g;
    DataLoader D;
    
    std::string type,path;
    std::cin>>type>>path;
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }
    
    ASSERT_EQ(D.load_data(g,my_type,path.c_str()),true); 

    ASSERT_EQ(g->triangle_counting(), 7515023);

    ASSERT_EQ(g->triangle_counting_mt(24), 7515023);
}

