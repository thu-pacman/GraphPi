#include "../include/restricts_generator.h"
#include<assert.h>

void restricts_generate(const Schedule &_schedule, int thread_num, std::vector< std::vector< std::pair<int,int> > > &restricts) {
    Schedule schedule(_schedule.get_adj_mat_ptr(), _schedule.get_size());
    schedule.aggressive_optimize_get_all_pairs(restricts);
    int size = schedule.get_size();
    Graph* complete;
    DataLoader D;
    assert(D.load_data(complete, size + 1));
    long long ans = complete->pattern_matching( schedule, thread_num) / schedule.get_multiplicity();
    for(int i = 0; i < restricts.size(); ) {
        Schedule cur_schedule(schedule.get_adj_mat_ptr(), schedule.get_size());
        cur_schedule.add_restrict(restricts[i]);
        long long cur_ans = complete->pattern_matching( cur_schedule, thread_num);
        if( cur_ans != ans) {
            restricts.erase(restricts.begin() + i);
        }
        else {
            ++i;
        }
    }

    delete complete;
}

