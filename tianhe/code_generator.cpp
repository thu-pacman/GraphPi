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
        if (i.find("Graphmpi") != string::npos) out << "//";
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

void code_gen(Graph* g, const Pattern &pattern, int performance_modeling_type, int restricts_type, std::string src_path, bool use_in_exclusion_optimize, char *argv[]) {
    using namespace std;
    bool is_pattern_valid;
    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, g->tri_cnt);
    assert(is_pattern_valid);
    fstream fi(src_path.c_str());
    vector<vector<string> > blocks;
    string def;
    string baseline0 = "\n\n#include \"../include/graph.h\"\n\n#include \"../include/dataloader.h\"\n\n#include \"../include/pattern.h\"\n\n#include \"../include/schedule.h\"\n\n#include \"../include/common.h\"\n\n#include \"../include/motif_generator.h\"\n\n\n\n#include <assert.h>\n\n#include <iostream>\n\n#include <string>\n\n#include <algorithm>\n\n\n\nlong long Graph::unfold_pattern_matching(const Schedule& schedule, int thread_count, bool clique)\n\n{\n\n    long long global_ans = 0;\n\n#pragma omp parallel num_threads(thread_count) reduction(+: global_ans)\n\n    {\n\n        double start_time = get_wall_time();\n\n        double current_time;\n\n        VertexSet* vertex_set = new VertexSet[schedule.get_total_prefix_num()];\n\n        VertexSet subtraction_set;\n\n        VertexSet tmp_set;\n\n        subtraction_set.init();\n\n        long long local_ans = 0;\n\n        // TODO : try different chunksize\n\n#pragma omp for schedule(dynamic) nowait\n\n        for (int vertex = 0; vertex < v_cnt; ++vertex)\n\n        {\n\n            unsigned int l, r;\n\n            get_edge_index(vertex, l, r);\n\n            for (int prefix_id = schedule.get_last(0); prefix_id != -1; prefix_id = schedule.get_next(prefix_id))\n\n            {\n\n                vertex_set[prefix_id].build_vertex_set(schedule, vertex_set, &edge[l], (int)r - l, prefix_id);\n\n            }\n\n            //subtraction_set.insert_ans_sort(vertex);\n\n            subtraction_set.push_back(vertex);\n\n            //if (schedule.get_total_restrict_num() > 0 && clique == false)\n\n            if(true)\n\n                unfold_pattern_matching_aggressive_func(schedule, vertex_set, subtraction_set, tmp_set, local_ans, 1);\n\n            /*else\n\n                pattern_matching_func(schedule, vertex_set, subtraction_set, local_ans, 1, clique);*/\n\n            subtraction_set.pop_back();\n\n            if( (vertex & (-vertex)) == (1<<15) ) {\n\n                current_time = get_wall_time();\n\n                if( current_time - start_time > max_running_time) {\n\n                    printf(\"TIMEOUT!\\n\");\n\n                    fflush(stdout);\n\n                    assert(0);\n\n                }\n\n            }\n\n        }\n\n        delete[] vertex_set;\n\n        // TODO : Computing multiplicty for a pattern\n\n        global_ans += local_ans;\n\n        \n\n    }\n\n    return global_ans;\n\n}\n";
    string baseline1 = "\n\nvoid test_pattern(Graph* g, const Pattern &pattern, int performance_modeling_type, int restricts_type, bool use_in_exclusion_optimize) {\n\n    int thread_num = 24;\n\n    double t1,t2;\n\n    \n\n    bool is_pattern_valid;\n\n    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, g->tri_cnt);\n\n    assert(is_pattern_valid);\n\n\n\n    t1 = get_wall_time();\n\n    long long ans = g->unfold_pattern_matching(schedule, thread_num);\n\n    t2 = get_wall_time();\n\n\n\n    printf(\"ans %lld,%.6lf\\n\", ans, t2 - t1);\n\n    schedule.print_schedule();\n\n    const auto& pairs = schedule.restrict_pair;\n\n    printf(\"%d \", int(pairs.size()));\n\n    for(auto& p : pairs)\n\n        printf(\"(%d,%d)\",p.first,p.second);\n\n    puts(\"\");\n\n    fflush(stdout);\n\n\n\n}\n\n\n\nint main(int argc,char *argv[]) {\n\n    Graph *g;\n\n    DataLoader D;\n";
    string baseline2 = "\n\n    DataType my_type;\n\n    \n\n    GetDataType(my_type, type);\n\n\n\n    if(my_type == DataType::Invalid) {\n\n        printf(\"Dataset not found!\\n\");\n\n        return 0;\n\n    }\n\n\n\n    assert(D.load_data(g,my_type,path.c_str())==true); \n\n\n\n    printf(\"Load data success!\\n\");\n\n    fflush(stdout);\n\n\n\n    Pattern p(size, adj_mat);\n\n    test_pattern(g, p, test_type, test_type, use_in_exclusion_optimize);\n\n    \n\n    delete g;\n\n}\n";
    auto fun = [](string &str) {
        for (int i = 0; i < int(str.size()) - 1; i++) {
            if (str.substr(i, 2) == "\n\n") str.replace(i, 2, "\n");
        }
    };
    fun(baseline0);
    fun(baseline1);
    fun(baseline2);
    for (int in_fun = 0, in_block = 0;;) {
        string tmp;
        getline(fi, tmp);
        if (fi.eof()) break;
        if (tmp.size() >= 3u && tmp.find("###") != string::npos) {
            in_fun = !in_fun;
            if (in_fun) def = tmp;
        }
        if (tmp.size() >= 3u && tmp.find("@@@") != string::npos) {
            in_block = !in_block;
            if (in_block) blocks.push_back(vector<string>());
        }
        if (in_block) blocks.rbegin()->push_back(tmp);
    }
    def.insert(12, "unfold_");
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
    cout << "*/\n";
    cout << baseline0;
    int indent = 0;
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
    cout << baseline1;
    cout << string("    const std::string type = \"") + argv[1] + "\";\n";
    cout << string("    const std::string path = \"") + argv[2] + "\";\n";
    cout << string("    int size = ") + argv[3] + ";\n";
    cout << string("    char adj_mat[] = \"") + argv[4] + "\";\n";
    cout << string("    int test_type = ") + argv[5] + ";\n";
    cout << string("    bool use_in_exclusion_optimize = ") + argv[7] + ";\n";
    cout << baseline2;
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
    code_gen(g, p, test_type, test_type, std::string(argv[6]), atoi(argv[7]), argv);
    
    delete g;
}
