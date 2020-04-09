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
   
    int size = atoi(argv[1]);
    char* adj_mat = argv[2];

    Pattern pattern(size, adj_mat);
    bool is_valid;
    Schedule schedule(pattern, is_valid, 0, 0, 0, 0, 0, 0);
    std::vector< std::vector< std::pair<int,int> > > restricts;
    schedule.restricts_generate(schedule.get_adj_mat_ptr(), restricts);
    schedule.print_schedule();
    for(auto &restrict : restricts) {
        for(auto &p : restrict)
            printf("(%d,%d)",p.first,p.second);
        puts("");
    }
    return 0;
}
