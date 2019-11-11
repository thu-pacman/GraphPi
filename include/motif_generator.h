#pragma once
#include "pattern.h"
#include <vector>

class MotifGenerator
{
public:
    MotifGenerator(int input_size): size(input_size){}
    std::vector<Pattern> generate();
private:
    int size;
    std::set< std::set<int> > s;
    void generate_all_pattern(std::vector<Pattern>& vec, std::set<int> edge_set, Pattern& p, int depth, int i, int j);
};