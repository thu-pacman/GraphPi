#pragma once
#include <sys/time.h>
#include <cstdlib>
#include "pattern.h"

double get_wall_time(); 

void PatternType_printer(PatternType type);

bool is_equal_adj_mat(const int* adj_mat1, const int* adj_mat2, int size);
