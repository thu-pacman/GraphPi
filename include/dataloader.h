#pragma once
#include "./graph.h"
#include <map>
#include <algorithm>


enum DataType {
    Patents,
    Orkut,
    complete8
};

class DataLoader {
public:

    bool load_data(Graph* &g, DataType type, const char* path);

private:
    static bool cmp_pair(std::pair<int,int>a, std::pair<int,int>b);
    std::map<int,int> id;
};
