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

int main(int argc,char *argv[]) {
    Graph *g;
    DataLoader D;

    std::string type = "Patents";
    std::string path = "/home/zms/patents_input";
    DataType my_type;
    if(type == "Patents") my_type = DataType::Patents;
    else {
        printf("invalid DataType!\n");
    }

    assert(D.load_data(g,my_type,path.c_str())==true); 

    printf("Load data success!\n");
    fflush(stdout);
    
    Pattern pattern(PatternType::House);
    bool is_valid;
    Schedule schedule(pattern, is_valid, 0, 0, 0, 0);
    std::vector< std::vector< std::pair<int,int> > > restricts;
    schedule.restricts_generate(schedule.get_adj_mat_ptr(), restricts);
    schedule.print_schedule();
    for(auto &restrict : restricts) {
        for(auto &p : restrict)
            printf("(%d,%d)",p.first,p.second);
        puts("");
    }

    delete g;
}
