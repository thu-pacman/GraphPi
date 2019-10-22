class Graph {
public:
	int v_cnt; // number of vertex
	int e_cnt; // number of edge
	
	int *edge; // edges
	int *vertex; // v_i's neighbor is in edge[ vertex[i], vertex[i+1]-1]
	

	Graph() {
		v_cnt = e_cnt = 0;
		edge = vertex = nullptr;
	}

	~Graph() {
		delete[] edge;
		delete[] vertex;
	}

	void load_patent();

	int intersection_size(int v1,int v2);

	int triangle_counting();
};
