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
    
    Pattern pattern(PatternType::House);
    printf("5 ");
    const int* adj_mat = pattern.get_adj_mat_ptr();
    for(int i = 0; i < 5; ++i)
        for(int j = 0; j< 5; ++j)
            printf("%d", adj_mat[INDEX(i,j,5)]);
    puts("");
    Pattern p(PatternType::Cycle_6_Tri);
    printf("6 ");
    const int* adj_mat2 = p.get_adj_mat_ptr();
    for(int i = 0; i < 6; ++i)
        for(int j = 0; j< 6; ++j)
            printf("%d", adj_mat2[INDEX(i,j,6)]);
    return 0;
}
