#pragma once
#include <sys/time.h>
#include <cstdlib>
#include <string>
#include "pattern.h"
#include "dataloader.h"
#include "schedule.h"

double get_wall_time(); 

void PatternType_printer(PatternType type);

bool is_equal_adj_mat(const int* adj_mat1, const int* adj_mat2, int size);

void GetDataType(DataType &type, const std::string str);

int read_int();

unsigned int read_unsigned_int();
