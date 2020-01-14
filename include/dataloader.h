#pragma once
#include "./graph.h"
#include <map>
#include <algorithm>


enum DataType {
    Patents,
    Orkut,
    complete8,
    LiveJournal
};

class DataLoader {
public:

    bool load_data(Graph* &g, DataType type, const char* path, int oriented_type = 0);

private:
    static bool cmp_pair(std::pair<int,int>a, std::pair<int,int>b);
    static bool cmp_degree_gt(std::pair<int,int> a,std::pair<int,int> b);
    static bool cmp_degree_lt(std::pair<int,int> a,std::pair<int,int> b);
    std::map<int,int> id;
};
