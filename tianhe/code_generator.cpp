#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"
#include "../include/motif_generator.h"

#include <assert.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>

void print_block(std::ostream &out, std::vector<std::string> block, int indent) {
    using namespace std;
    int mi = 0x7fffffff;
    for (string i : block) {
        int k = 0;
        for (; k < int(i.size()) && i[k] == ' '; k++);
        if (k < int(i.size())) mi = min(mi, k);
    }
    for (string i : block) {
        if (int(i.size()) <= mi) {
            out.put('\n');
            continue;
        }
        for (int j = 0; j < indent; j++) out.put(' ');
        out << i.substr(mi) << endl;
    }
}

void print_str(std::ostream &out, std::string str, int indent) {
    using namespace std;
    vector<string> tmp;
    tmp.push_back(str);
    print_block(out, tmp, indent);
}

void code_gen(Graph* g, const Pattern &pattern, int performance_modeling_type, int restricts_type, std::string src_path, bool use_in_exclusion_optimize = false) {
    using namespace std;
    bool is_pattern_valid;
    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, g->tri_cnt);
    assert(is_pattern_valid);
    fstream fi(src_path.c_str());
    vector<string> before, after;
    vector<vector<string> > blocks;
    string def;
    for (int in_fun = 0, in_block = 0;;) {
        string tmp;
        getline(fi, tmp);
        if (fi.eof()) break;
        if (tmp.size() >= 3u && tmp.find("###") != string::npos) {
            in_fun++;
            if (in_fun == 1) def = tmp;
        }
        if (tmp.size() >= 3u && tmp.find("@@@") != string::npos) {
            in_block = !in_block;
            if (in_block) blocks.push_back(vector<string>());
        }
        if (!in_fun) before.push_back(tmp);
        if (in_fun == 2) after.push_back(tmp);
        if (in_block) blocks.rbegin()->push_back(tmp);
    }
    fi.close();
    vector<string> finale, before_re, after_re;
    int max_depth;
    if (schedule.get_in_exclusion_optimize_num() >= 1) {
        finale = blocks[1]; 
        max_depth = schedule.get_size() - schedule.get_in_exclusion_optimize_num();
    }
    else {
        finale = blocks[2];
        max_depth = schedule.get_size() - 1;
    }
    blocks[3].insert(blocks[3].begin(), blocks[0].begin(), blocks[0].end());
    before_re = blocks[3];
    after_re = blocks[4];
    /*for (int i = 0; i < blocks.size(); i++) {
        cout << "\n\nblock" + to_string(i) + "\n";
        print_block(cout, blocks[i], 0);
    }*/
    std::cout << "*/\n";
    int indent = 0;
    print_block(cout, before, indent);
    print_str(cout, def, indent);
    print_str(cout, "{", indent);
    indent += 4;
    for (int depth = 1; depth < max_depth; depth++) {
        if (depth > 1) print_str(cout, "const int depth = " + to_string(depth) + ";", indent);
        print_block(cout, before_re, indent);
        indent += 4;
        print_str(cout, "{", indent);
        indent += 4;
    }
    print_str(cout, "const int depth = " + to_string(max_depth) + ";", indent);
    print_block(cout, finale, indent);
    for (int depth = max_depth - 1; depth; depth--) {
        indent -= 4;
        print_str(cout, "}", indent);
        indent -= 4;
        print_block(cout, after_re, indent);
    }
    indent -= 4;
    print_str(cout, "}", indent);
    print_block(cout, after, indent);
}

int main(int argc,char *argv[]) {
    std::cout << "/*\n";
    Graph *g;
    DataLoader D;

    const std::string type = argv[1];
    const std::string path = argv[2];
    
    int size = atoi(argv[3]);
    char* adj_mat = argv[4];

    int test_type = atoi(argv[5]);
    
    DataType my_type;
    
    GetDataType(my_type, type);

    if(my_type == DataType::Invalid) {
        std::cout << "Dataset not found!" << std::endl;
        return 0;
    }

    assert(D.load_data(g,my_type,path.c_str())==true); 

    std::cout << "Load data success!" << std::endl;

    Pattern p(size, adj_mat);
    code_gen(g, p, test_type, test_type, std::string(argv[6]), atoi(argv[7]));
    
    delete g;
}
