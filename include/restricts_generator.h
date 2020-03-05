#pragma once
#include "graph.h"
#include "dataloader.h"
#include "schedule.h"

#include<vector>

void restricts_generate(const Schedule &schedule, int thread_num, std::vector< std::vector< std::pair<int,int> > > &restricts);
