#pragma once
#include <sys/time.h>
#include <cstdlib>
#include "pattern.h"

double get_wall_time(); 

void PatternType_printer(PatternType type) {
    if(type == PatternType::Rectangle) {
        printf("Rectangle\n");
    }
    if(type == PatternType::QG3) {
        printf("QG3\n");
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

void PatternType_printer(PatternType type);

bool is_equal_adj_mat(const int* adj_mat1, const int* adj_mat2, int size);
