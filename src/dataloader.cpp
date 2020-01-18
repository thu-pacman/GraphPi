#include "../include/dataloader.h"
#include "../include/graph.h"
#include "../include/vertex_set.h"
#include <cstdlib>
#include <cstring>

bool DataLoader::load_data(Graph* &g, DataType type, const char* path, int pattern_size, int max_pattern_degree, int pattern_diameter, std::vector<long long> &graph_degree_info, std::vector<long long> &graph_size_info, int oriented_type) {
    if(type == Patents || type == Orkut || type == complete8 || type == LiveJournal) {
        if (freopen(path, "r", stdin) == NULL)
        {
            printf("File not found. %s\n", path);
            return false;
        }
        printf("Load begin in %s\n",path);
        g = new Graph();
        scanf("%d%d",&g->v_cnt,&g->e_cnt);
        int* degree = new int[g->v_cnt];
        memset(degree, 0, g->v_cnt * sizeof(int));
        g->e_cnt *= 2;
        std::pair<int,int> *e = new std::pair<int,int>[g->e_cnt];
        id.clear();
        int x,y;
        int tmp_v,tmp_e;
        tmp_v = 0;
        tmp_e = 0;
        while(scanf("%d%d",&x,&y)!=EOF) {
            if(x == y) {
                printf("find self circle\n");
                g->e_cnt -=2;
                continue;
                //return false;
            }
            if(!id.count(x)) id[x] = tmp_v ++;
            if(!id.count(y)) id[y] = tmp_v ++;
            x = id[x];
            y = id[y];
            e[tmp_e++] = std::make_pair(x,y);
            e[tmp_e++] = std::make_pair(y,x);
            ++degree[x];
            ++degree[y];
            if(tmp_e % 1000000 == 0) printf("load %d edges\n",tmp_e);
        }
        if ( oriented_type != 0 ) {
            std::pair<int,int> *rank = new std::pair<int,int>[g->v_cnt];
            int *new_id = new int[g->v_cnt];
            for(int i = 0; i < g->v_cnt; ++i) rank[i] = std::make_pair(i,degree[i]);
            if( oriented_type == 1) std::sort(rank, rank + g->v_cnt, cmp_degree_gt);
            if( oriented_type == 2) std::sort(rank, rank + g->v_cnt, cmp_degree_lt);
            for(int i = 0; i < g->v_cnt; ++i) new_id[rank[i].first] = i;
//            for(int i = 0; i < g->v_cnt; ++i) 
//                if( rank[i].second < rank[i + 1].second ) puts("wrong");
            for(int i = 0; i < g->e_cnt; ++i) {
                e[i].first = new_id[e[i].first];
                e[i].second = new_id[e[i].second];
            }
            delete[] rank;
            delete[] new_id;
        }
        std::sort(degree, degree + g->v_cnt);

        // get graph degree info for performancel modeling
        graph_degree_info.clear();
        graph_degree_info.push_back(g->v_cnt);
        graph_degree_info.push_back(g->e_cnt);
        for(int k = 2; k <= max_pattern_degree; ++k) {
            long long val = 0;
            for(int i = 0; i < g->v_cnt; ++i) {
                val += comb(degree[i],k);
                if( val < 0 ) printf("too big when get graph info\n");
            }
            graph_degree_info.push_back(val);
        }
        for(int k = 0; k <= max_pattern_degree; ++k)
            printf("graph_degree_info %d: %lld\n", k, graph_degree_info[k]);

        // The max size of intersections is the second largest degree.
        VertexSet::max_intersection_size = degree[g->v_cnt - 2];
        delete[] degree;
        if(tmp_v != g->v_cnt) {
            printf("vertex number error!\n");
        }
        if(tmp_e != g->e_cnt) {
            printf("edge number error!\n");
        }
        if(tmp_v != g->v_cnt || tmp_e != g->e_cnt) {
            fclose(stdin);
            delete g;
            delete[] e;
            return false;
        }
        std::sort(e,e+tmp_e,cmp_pair);
        g->e_cnt = unique(e,e+tmp_e) - e;
        for(int i = 0; i < g->e_cnt - 1; ++i)
            if(e[i] == e[i+1]) {
                printf("have same edge\n");
                fclose(stdin);
                delete g;
                delete[] e;
                return false;
            }
        g->edge = new int[g->e_cnt];
        g->vertex = new int[g->v_cnt + 1];
        int lst_v = -1;
        for(int i = 0; i < g->v_cnt; ++i) g->vertex[i] = -1;
        for(int i = 0; i < g->e_cnt; ++i) {
            if(e[i].first != lst_v)
                g->vertex[e[i].first] = i;
            lst_v = e[i].first;
            g->edge[i] = e[i].second;
        }
        delete[] e;
        printf("Success! There are %d nodes and %d edges.\n",g->v_cnt,g->e_cnt);
        fflush(stdout);
        if(g->vertex[g->v_cnt - 1] == -1)
            g->vertex[g->v_cnt - 1] = g->e_cnt;
        g->vertex[g->v_cnt] = g->e_cnt;
        for(int i = g->v_cnt - 2; i >= 0; --i)
            if(g->vertex[i] == -1) {
                g->vertex[i] = g->vertex[i+1];
            
            }
        //get graph size info for performance modeling
        int* vis = new int[g->v_cnt];
        for(int i = 0; i < g->v_cnt; ++i) vis[i] = 0;
        int vis_clock = 0;
        int* bfs_q = new int[g->v_cnt];
        int bfs_q_head = 0;
        int bfs_q_tail = 0;
        int *dis = new int[g->v_cnt];
        
        graph_size_info.clear();
        graph_size_info.push_back(1);
        graph_size_info.push_back(g->v_cnt);
        for(int i = 2; i < pattern_size; ++i)
            graph_size_info.push_back(0);

        for(int root = 0; root < g->v_cnt; ++root) {
            int sum = 0;

            vis_clock++;
            bfs_q_head = bfs_q_tail = 0;
            
            bfs_q[bfs_q_tail++] = root;
            vis[root] = vis_clock;
            dis[root] = 0;
            while( bfs_q_head < bfs_q_tail ) {
                int cur = bfs_q[ bfs_q_head++ ];
                ++sum;
                if( dis[cur] == pattern_diameter) continue;
                for(int i = g->vertex[cur]; i < g->vertex[cur + 1]; ++i)
                    if( vis[g->edge[i]] != vis_clock) {
                        vis[g->edge[i]] = vis_clock;
                        dis[g->edge[i]] = dis[cur] + 1;
                        bfs_q[bfs_q_tail++] = g->edge[i];
                    }
            }

            for(int i = 2; i < pattern_size; ++i) 
                graph_size_info[i] += comb(sum - 1, i - 1);
        }

        for(long long i = 2; i < pattern_size; ++i)
            graph_size_info[i] /= i;
            

        delete[] vis;
        delete[] bfs_q;
        delete[] dis;
        return true;
    }
    printf("invalid DataType!\n");
    return false;
}

bool DataLoader::cmp_pair(std::pair<int,int>a, std::pair<int,int>b) {
    return a.first < b.first || (a.first == b.first && a.second < b.second);
}

bool DataLoader::cmp_degree_gt(std::pair<int,int> a,std::pair<int,int> b) {
    return a.second > b.second;
}

bool DataLoader::cmp_degree_lt(std::pair<int,int> a,std::pair<int,int> b) {
    return a.second < b.second;
}

long long DataLoader::comb(int n, int k) {
    long long ans = 1;
    for(int i = n; i > n - k; --i)
        ans = ans * i;
    for(int i = 1; i <= k; ++i)
        ans = ans / k;
    return ans;
}
