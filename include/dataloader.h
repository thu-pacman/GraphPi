#pragma once
#include "./graph.h"
#include <map>
#include <algorithm>
#include <vector>

enum DataType {
    Patents,
    Orkut,
    complete8,
    LiveJournal
};

class DataLoader {
public:

    bool load_data(Graph* &g, DataType type, const char* path, int pattern_size, int max_pattern_degree, int pattern_diameter, std::vector<long long> &graph_degree_info, std::vector<long long> &graph_size_info, int oriented_type = 0);

private:
    static bool cmp_pair(std::pair<int,int>a, std::pair<int,int>b);
    static bool cmp_degree_gt(std::pair<int,int> a,std::pair<int,int> b);
    static bool cmp_degree_lt(std::pair<int,int> a,std::pair<int,int> b);

    long long comb(int n,int k);

    std::map<int,int> id;
};
