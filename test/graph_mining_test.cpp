#include <gtest/gtest.h>
#include <../include/graph.h>

TEST(graph_mining_test, test_triangle_counting) {
    Graph g;
    g.load_patent();

    ASSERT_EQ(g.triangle_counting(), 7515023);
}

