#include <cstdio>
#include <map>
#include <algorithm>

int v_cnt;
int e_cnt;

int *edge;
int *vertex;

std::map<int,int> id;

bool cmp_pair(std::pair<int,int>a, std::pair<int,int>b) {
	return a.first < b.first || (a.first == b.first && a.second < b.second);
}

bool load_patent() {
	printf("Load Patent begin\n");
	freopen("patents_input","r",stdin);
	scanf("%d%d",&v_cnt,&e_cnt);
	e_cnt *= 2;
	std::pair<int,int> *e = new std::pair<int,int>[e_cnt];
	id.clear();
	int x,y;
	int tmp_v,tmp_e;
	tmp_v = 0;
	tmp_e = 0;
	while(scanf("%d%d",&x,&y)!=EOF) {
		if(x == y) {
			printf("x = y");
			e_cnt -=2;
			continue;
			//return false;
		}
		if(!id.count(x)) id[x] = tmp_v ++;
		if(!id.count(y)) id[y] = tmp_v ++;
		x = id[x];
		y = id[y];
		e[tmp_e++] = std::make_pair(x,y);
		e[tmp_e++] = std::make_pair(y,x);
		if(tmp_e % 1000000 == 0) printf("load %d edges\n",tmp_e);
	}
	if(tmp_v != v_cnt) {
		printf("vertex number error!\n");
	}
	if(tmp_e != e_cnt) {
		printf("edge number error!\n");
	}
	if(tmp_v != v_cnt || tmp_e != e_cnt) 
		return false;
	std::sort(e,e+tmp_e,cmp_pair);
	e_cnt = unique(e,e+tmp_e) - e;
	for(int i = 0; i < e_cnt - 1; ++i)
		if(e[i] == e[i+1]) {
			printf("have same edge");
			return false;
		}
	edge = new int[e_cnt];
	vertex = new int[v_cnt];
	int lst_v = -1;
	for(int i = 0; i < v_cnt; ++i) vertex[i] = -1;
	for(int i = 0; i < e_cnt; ++i) {
		if(e[i].first != lst_v) vertex[e[i].first] = i;
		lst_v = e[i].first;
		edge[i] = e[i].second;
	}
	delete[] e;
	printf("Success! There are %d nodes and %d edges.\n",v_cnt,e_cnt);
	if(vertex[v_cnt - 1] == -1) vertex[v_cnt - 1] = e_cnt;
	for(int i = v_cnt - 2; i >= 0; --i)
		if(vertex[i] == -1) {
			vertex[i] = vertex[i+1];
		}
	for(int i = 1; i < vertex[i]; ++i)
		if(vertex[i] < vertex[i-1]) {
			puts("error");
			return false;
		}

	return true;
}

int main() {
	if(load_patent() == false) {
		printf("Load Patent Failed.\n");
		return 0;
	}
	else {
		printf("Load Patent Success.\n");
	}
	printf("Write to standard_input\n");
	freopen("standard_input","w",stdout);
	printf("%d %d\n",v_cnt,e_cnt);
	for(int i = 0; i < v_cnt; ++i)
		printf("%d\n",vertex[i]);
	for(int i = 0; i < e_cnt; ++i)
		printf("%d\n",edge[i]);
	fclose(stdout);
	delete[] vertex;
	delete[] edge;
	return 0;
}
