#include "../include/motif_generator.h"
#include <vector>
#include <set>

void MotifGenerator::generate_all_pattern(std::vector<Pattern>& vec, std::set<int> edge_set, Pattern& p, int depth, int i, int j)
{
    if (i == size - 1)
    {
        // its isomorphism graph has not been generated and it's connected
        if (s.count(edge_set) == 0 && p.check_connected())
        {
            vec.push_back(p);
            p.count_all_isomorphism(s);
        }
        return;
    }
    //use edge(i,j)
    edge_set.insert(i * size + j);
    p.add_edge(i, j);
    if (j == size - 1)
        generate_all_pattern(vec, edge_set, p, depth + 1, i + 1, i + 2);
    else
        generate_all_pattern(vec, edge_set, p, depth + 1, i, j + 1);
    p.del_edge(i, j);
    edge_set.erase(i * size + j);
    //not use edge(i,j)
    if (j == size - 1)
        generate_all_pattern(vec, edge_set, p, depth + 1, i + 1, i + 2);
    else
        generate_all_pattern(vec, edge_set, p, depth + 1, i, j + 1);
}

std::vector<Pattern> MotifGenerator::generate()
{
    s.clear();
    std::set<int> edge_set;
    edge_set.clear();

    std::vector<Pattern> vec;
    vec.clear();
    Pattern p(size);
    generate_all_pattern(vec, edge_set, p, 0, 0, 1);
    return vec;
}