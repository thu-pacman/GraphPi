SET(GraphMiningSrc
TestClass.cpp
graph.cpp
dataloader.cpp
pattern.cpp
prefix.cpp
schedule.cpp
vertex_set.cpp
motif_generator.cpp
graphmpi.cpp
common.cpp
disjoint_set_union.cpp
)

ADD_LIBRARY(graph_mining SHARED ${GraphMiningSrc}) 

ADD_EXECUTABLE(performance_compare_test performance_compare_test.cpp)
TARGET_LINK_LIBRARIES(performance_compare_test graph_mining)

#ADD_EXECUTABLE(run_brute_force_tc run_brute_force_tc.cpp)
#TARGET_LINK_LIBRARIES(run_brute_force_tc graph_mining)

ADD_EXECUTABLE(run_general_tc run_general_tc.cpp)
TARGET_LINK_LIBRARIES(run_general_tc graph_mining)
