#include "common.h"
#include <sys/time.h>
#include <cstdlib>

double get_wall_time() {
    struct timeval time;
    if(gettimeofday(&time,NULL)) {
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * 0.000001;
}

void PatternType_printer(PatternType type) {
    if(type == PatternType::Rectangle) {
        printf("Rectangle\n");
    }
    if(type == PatternType::Pentagon) {
        printf("Pentagon\n");
    }
    if(type == PatternType::House) {
        printf("House\n");
    }
    if(type == PatternType::Hourglass) {
        printf("Hourglass\n");
    }
    if(type == PatternType::Cycle_6_Tri) {
        printf("Cycle_6_Tri\n");
    }
    if(type == PatternType::Clique_7_Minus) {
        printf("Clique_7_Minus\n");
    }
}

bool is_equal_adj_mat(const int* adj_mat1, const int* adj_mat2, int size) {
    for(int i = 0; i < size; ++i)
        for(int j = 0; j < size; ++j)
            if(adj_mat1[INDEX(i,j,size)] != adj_mat2[INDEX(i,j,size)]) 
                return false;
    return true;
}
