enable_testing()

find_package(GTest)
if (GTEST_FOUND)
    INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
#    ADD_EXECUTABLE(graph_mining_test schedule_test.cpp graph_mining_test.cpp)
#    TARGET_LINK_LIBRARIES(graph_mining_test
#                        ${GTEST_BOTH_LIBRARIES}
#                        graph_mining
#                        pthread
#                        gomp)
#    gtest_discover_tests(graph_mining_test)

#    ADD_EXECUTABLE(order_test order_test.cpp)
#    TARGET_LINK_LIBRARIES(order_test
#                        ${GTEST_BOTH_LIBRARIES}
#                        graph_mining
#                        pthread
#                        gomp)
#    gtest_discover_tests(order_test)
    
#    ADD_EXECUTABLE(patents_qg5_test patents_qg5_test.cpp)
#    TARGET_LINK_LIBRARIES(patents_qg5_test
#                        ${GTEST_BOTH_LIBRARIES}
#                        graph_mining
#                        pthread
#                        gomp)
#    gtest_discover_tests(patents_qg5_test)

    ADD_EXECUTABLE(performance_test_patents_qg5 performance_test_patents_qg5.cpp)
    TARGET_LINK_LIBRARIES(performance_test_patents_qg5
                        ${GTEST_BOTH_LIBRARIES}
                        graph_mining
                        pthread
                        gomp)
    gtest_discover_tests(performance_test_patents_qg5)
endif()

