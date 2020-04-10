enable_testing()

find_package(GTest)
if (GTEST_FOUND)
    INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
#    ADD_EXECUTABLE(graph_mining_test gtest_main.cpp schedule_test.cpp graph_mining_test.cpp)
#    TARGET_LINK_LIBRARIES(graph_mining_test
#                        ${GTEST_LIBRARIES}
#                        graph_mining
#                        pthread
#                        gomp)
#    gtest_discover_tests(graph_mining_test)

#    ADD_EXECUTABLE(pattern_matching_test pattern_matching_test.cpp)
#    TARGET_LINK_LIBRARIES(pattern_matching_test
#                        ${GTEST_BOTH_LIBRARIES}
#                        graph_mining
#                        pthread
#                        gomp)
#    gtest_discover_tests(pattern_matching_test)
    
#    ADD_EXECUTABLE(performance_modeling_test performance_modeling_test.cpp)
#    TARGET_LINK_LIBRARIES(performance_modeling_test
#                        ${GTEST_BOTH_LIBRARIES}
#                        graph_mining
#                        pthread
#                        gomp)
#    gtest_discover_tests(performance_modeling_test)
    
#    ADD_EXECUTABLE(restricts_generation_test restricts_generation_test.cpp)
#    TARGET_LINK_LIBRARIES(restricts_generation_test
#                        ${GTEST_BOTH_LIBRARIES}
#                        graph_mining
#                        pthread
#                        gomp)
#    gtest_discover_tests(restricts_generation_test)
    
#    ADD_EXECUTABLE(schedule_compare_test schedule_compare_test.cpp)
#    TARGET_LINK_LIBRARIES(schedule_compare_test
#                        ${GTEST_BOTH_LIBRARIES}
#                        graph_mining
#                        pthread
#                        gomp)
#    gtest_discover_tests(schedule_compare_test)
    
#    ADD_EXECUTABLE(restricts_compare_test restricts_compare_test.cpp)
#    TARGET_LINK_LIBRARIES(restricts_compare_test
#                        ${GTEST_BOTH_LIBRARIES}
#                        graph_mining
#                        pthread
#                        gomp)
#    gtest_discover_tests(restricts_compare_test)
    
#    ADD_EXECUTABLE(performance_compare_test performance_compare_test.cpp)
#    TARGET_LINK_LIBRARIES(performance_compare_test
#                        ${GTEST_BOTH_LIBRARIES}
#                        graph_mining
#                        pthread
#                        gomp)
#    gtest_discover_tests(performance_compare_test)
endif()

