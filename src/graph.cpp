#include "../include/graph.h"
#include <cstdio>

#include<sys/time.h>
#include<unistd.h>
#include<cstdlib>

void Graph::load_patent() {
	//in standard_input file:
	//Nodes: 3774768 Edges: 16518948 Triangles: 7515023
//	system("pwd");
	freopen("../../test/standard_input","r",stdin);
	scanf("%d%d",&v_cnt,&e_cnt);
	vertex = new int[v_cnt];
	edge = new int[e_cnt];
	for(int i = 0; i < v_cnt; ++i)
		scanf("%d",&vertex[i]);
	for(int i = 0; i < e_cnt; ++i)
		scanf("%d",&edge[i]);
}

int Graph::intersection_size(int v1,int v2) {
	int l1 = vertex[v1];
	int r1 = (v1 == v_cnt -1 ? e_cnt : vertex[v1+1]);
	int l2 = vertex[v2];
	int r2 = (v2 == v_cnt -1 ? e_cnt : vertex[v2+1]);
	int ans = 0;
	while(l1 < r1 && l2 < r2) {
		if(edge[l1] < edge[l2]) {
			++l1;
		}
		else {
			if(edge[l2] < edge[l1]) {
				++l2;
			}
			else {
				++l1;
				++l2;
				++ans;
			}
		}
	}
	//puts("inter success!");
	return ans;
}

int Graph::triangle_counting() {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv,&tz);

	double t1 = tv.tv_sec + tv.tv_usec * 1e-6;
	int ans = 0;
	for(int v = 0; v < v_cnt; ++v) {
		// for v in G
		int l = vertex[v];
		int r = (v == v_cnt -1 ? e_cnt : vertex[v+1]);
		for(int v1 = l; v1 < r; ++v1) {
			//for v1 in N(v)
			ans += intersection_size(v,edge[v1]);
		}
	}
	ans /= 6;
	double t2 = tv.tv_sec + tv.tv_usec * 1e-6;
	printf("counting time: %.6lf\n", t2 - t1);
	return ans;
}
