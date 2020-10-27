#include "../include/dataloader.h"
#include "../include/graph.h"
#include "../include/vertex_set.h"
#include "../include/common.h"
#include <cstdlib>
#include <cstring>

bool DataLoader::load_data(Graph* &g, DataType type, const char* path, int oriented_type) {
    if(type == Patents || type == Orkut || type == complete8 || type == LiveJournal || type == MiCo || type == CiteSeer || type == Wiki_Vote) {
        return general_load_data(g, type, path, oriented_type);
    }

    if( type == Twitter) {
        return twitter_load_data(g, type, path, oriented_type);
    }
    printf("invalid DataType!\n");
    return false;
}

bool DataLoader::general_load_data(Graph *&g, DataType type, const char* path, int oriented_type) {
    if (freopen(path, "r", stdin) == NULL)
    {
        printf("File not found. %s\n", path);
        return false;
    }
    printf("Load begin in %s\n",path);
    g = new Graph();

    //load triangle counting information
    switch(type) {
        case DataType::Patents : {
            g->tri_cnt = Patents_tri_cnt;
            break;
        }
        case DataType::LiveJournal : {
            g->tri_cnt = LiveJournal_tri_cnt;
            break;
        }
        case DataType::MiCo : {
            g->tri_cnt = MiCo_tri_cnt;
            break;
        }
        case DataType::CiteSeer : {
            g->tri_cnt = CiteSeer_tri_cnt;
            break;
        }
        case DataType::Wiki_Vote : {
            g->tri_cnt = Wiki_Vote_tri_cnt;
            break;
        }
        case DataType::Orkut : {
            g->tri_cnt = Orkut_tri_cnt;
            break;
        }
        default : {
            g->tri_cnt = -1;
            break;
        }
    }

    scanf("%d%u",&g->v_cnt,&g->e_cnt);
    int* degree = new int[g->v_cnt];
    memset(degree, 0, g->v_cnt * sizeof(int));
    g->e_cnt *= 2;
    std::pair<int,int> *e = new std::pair<int,int>[g->e_cnt];
    id.clear();
    int x,y;
    int tmp_v;
    unsigned int tmp_e;
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
        //if(tmp_e % 1000000u == 0u) {
        //    printf("load %u edges\n",tmp_e);
        //    fflush(stdout);
        //}
    }

    // oriented_type == 0 do nothing
    //               == 1 high degree first
    //               == 2 low degree first
    if ( oriented_type != 0 ) {
        std::pair<int,int> *rank = new std::pair<int,int>[g->v_cnt];
        int *new_id = new int[g->v_cnt];
        for(int i = 0; i < g->v_cnt; ++i) rank[i] = std::make_pair(i,degree[i]);
        if( oriented_type == 1) std::sort(rank, rank + g->v_cnt, cmp_degree_gt);
        if( oriented_type == 2) std::sort(rank, rank + g->v_cnt, cmp_degree_lt);
        for(int i = 0; i < g->v_cnt; ++i) new_id[rank[i].first] = i;
        for(unsigned int i = 0; i < g->e_cnt; ++i) {
            e[i].first = new_id[e[i].first];
            e[i].second = new_id[e[i].second];
        }
        delete[] rank;
        delete[] new_id;
    }
    std::sort(degree, degree + g->v_cnt);

    // The max size of intersections is the second largest degree.
    //TODO VertexSet::max_intersection_size has different value with different dataset, but we use a static variable now.
    VertexSet::max_intersection_size = std::max( VertexSet::max_intersection_size, degree[g->v_cnt - 2]);
    g->max_degree = degree[g->v_cnt - 1];
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
    for(unsigned int i = 0; i < g->e_cnt - 1; ++i)
        if(e[i] == e[i+1]) {
            printf("have same edge\n");
            fclose(stdin);
            delete g;
            delete[] e;
            return false;
        }
    g->edge = new int[g->e_cnt];
    g->vertex = new unsigned int[g->v_cnt + 1];
    bool* have_edge = new bool[g->v_cnt];
    int lst_v = -1;
    for(int i = 0; i < g->v_cnt; ++i) have_edge[i] = false;
    for(unsigned int i = 0; i < g->e_cnt; ++i) {
        if(e[i].first != lst_v) {
            have_edge[e[i].first] = true;
            g->vertex[e[i].first] = i;
        }
        lst_v = e[i].first;
        g->edge[i] = e[i].second;
    }
    delete[] e;
    printf("Success! There are %d nodes and %u edges.\n",g->v_cnt,g->e_cnt);
    fflush(stdout);
    g->vertex[g->v_cnt] = g->e_cnt;
    for(int i = g->v_cnt - 1; i >= 0; --i)
        if(!have_edge[i]) {
            g->vertex[i] = g->vertex[i+1];
        }
    delete[] have_edge;

    return true;
}

bool DataLoader::twitter_load_data(Graph *&g, DataType type, const char* path, int oriented_type) {
    if (freopen(path, "r", stdin) == NULL)
    {
        printf("File not found. %s\n", path);
        return false;
    }
    printf("Load begin in %s\n",path);
    g = new Graph();
    g->tri_cnt = Twitter_tri_cnt;
    unsigned int* buffer = new unsigned int[41652230u + 2936729768u + 10u];
    FILE* file = fopen(path, "r");
    fread(buffer, sizeof(unsigned int), 41652230u + 2936729768u + 4, file);
    g->v_cnt = buffer[0];
    g->e_cnt = buffer[1];
    int mx_degree = buffer[2];
    VertexSet::max_intersection_size = std::max( VertexSet::max_intersection_size, mx_degree);
    g->max_degree = mx_degree;
    g->edge = new int [g->e_cnt];
    g->vertex = new unsigned int [g->v_cnt + 1];
    for(int i = 0; i < g->v_cnt + 1; ++i)
        g->vertex[i] = buffer[ 3 + i];
    for(unsigned int i = 0; i < g->e_cnt; ++i)
        g->edge[i] = buffer[4 + g->v_cnt + i];
    delete[] buffer;
    return true;
}

bool DataLoader::load_complete(Graph* &g, int clique_size) {
    g = new Graph();

    g->v_cnt = clique_size;
    g->e_cnt = clique_size * (clique_size - 1) / 2;

    int* degree = new int[g->v_cnt];
    memset(degree, 0, g->v_cnt * sizeof(int));
    g->e_cnt *= 2;
    std::pair<int,int> *e = new std::pair<int,int>[g->e_cnt];
    id.clear();
    int tmp_v;
    unsigned int tmp_e;
    tmp_v = 0;
    tmp_e = 0;
    for(int i = 0; i < clique_size; ++i)
        for(int j = 0; j < i; ++j) {
            int x = i, y = j;
            if(!id.count(x)) id[x] = tmp_v ++;
            if(!id.count(y)) id[y] = tmp_v ++;
            x = id[x];
            y = id[y];
            e[tmp_e++] = std::make_pair(x,y);
            e[tmp_e++] = std::make_pair(y,x);
            ++degree[x];
            ++degree[y];
        }

    std::sort(degree, degree + g->v_cnt);

    // The max size of intersections is the second largest degree.
    //TODO VertexSet::max_intersection_size has different value with different dataset, but we use a static variable now.
    VertexSet::max_intersection_size = std::max( VertexSet::max_intersection_size, degree[g->v_cnt - 2]);
    g->max_degree = degree[g->v_cnt - 1];
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
    g->edge = new int[g->e_cnt];
    g->vertex = new unsigned int[g->v_cnt + 1];
    bool* have_edge = new bool[g->v_cnt];
    int lst_v = -1;
    for(int i = 0; i < g->v_cnt; ++i) have_edge[i] = false;
    for(unsigned int i = 0; i < g->e_cnt; ++i) {
        if(e[i].first != lst_v) {
            have_edge[e[i].first] = true;
            g->vertex[e[i].first] = i;
        }
        lst_v = e[i].first;
        g->edge[i] = e[i].second;
    }
    delete[] e;
    g->vertex[g->v_cnt] = g->e_cnt;
    for(int i = g->v_cnt - 1; i >= 0; --i)
        if(!have_edge[i]) {
            g->vertex[i] = g->vertex[i+1];
        }
    delete[] have_edge;
    return true;
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
