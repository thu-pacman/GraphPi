enable_testing()
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    INCLUDE_DIRECTORIES(${OpenMP_INCLUDE_DIRS})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_CXX_FLAGS}" )
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenMP_CXX_FLAGS}" )
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}" )
    set(CMAKE_SHARE_LINKER_FLAGS "${CMAKE_SHARE_LINKER_FLAGS} ${OpenMP_SHARE_LINKER_FLAGS}" ) 
endif()

find_package(GTest)
if (GTEST_FOUND)
    INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS}
                        ${OpenMP_INCLUDE_DIRS})
    ADD_EXECUTABLE(graph_mining_test graph_mining_test.cpp)
    TARGET_LINK_LIBRARIES(graph_mining_test
                        ${GTEST_BOTH_LIBRARIES}
                        graph_mining
                        pthread
                        gomp)
    gtest_discover_tests(graph_mining_test)
endif()

