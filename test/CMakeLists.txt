enable_testing()

find_package(GTest)
if (GTEST_FOUND)
    INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
    ADD_EXECUTABLE(graph_mining_test gtest_main.cpp schedule_test.cpp graph_mining_test.cpp)
    TARGET_LINK_LIBRARIES(graph_mining_test
                        ${GTEST_LIBRARIES}
                        graph_mining
                        pthread
                        gomp)
    gtest_discover_tests(graph_mining_test)
endif()

