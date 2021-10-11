# GraphPi

GraphPi is being refactored. The official version will be released in the final version of our paper.



## How to run on a single machine

In this section, you will learn how to run GraphPi on a single machine with multi-threads (using OpenMP).



### Step 0 : Load the dataset

The class `DataLoader` is used to load the dataset(i.e., data graph).  And the dataset is stored in class `Graph` .

The format of the dataset may be a little different from its download version. The first line should contain two integers: the number of vertices and the number of edges respectively. From the second line, each line contains two integers `x`and ` y`, representing an undirected edge `(x,y)` in the graph. The wiki-vote dataset has been placed at GraphPi/dataset/wiki-vote_input as an example.

Since GraphPi's performance model needs the number of triangles of the dataset, if you want to use a new dataset, you need to add a new `DataType` and a corresponding constant variable named `dataset_tri_cnt` representing the number of triangles in `include/dataloader.h` (see our code for more detail). And we have already prepared these classical datasets for GraphPi in `include/dataloader.h`: `Patents` ,`Orkut` ,`LiveJournal`, `MiCo`, `Twitter`, `CiteSeer` and `WikiVote`.

When the dataset is prepared, we can load the dataset easily. First,  you need to define a pointer of a graph and a dataloader.

```cpp
Graph *g;
Dataloader D;
```

Then, you need to use `std::string` to assign the dataset's name and file path.

```cpp
const std::string type_str = "Wiki-Vote";
const std::string path = "./datasets/wiki-vote_input"
```

In `include/common.h`, we provide many functions to make your coding more easy. Using `GetDataType` to transform `std::string` to `DataType`.

```cpp
DataType my_type;
GetDataType(my_type, type_str);
```

Now you can load the dataset using the function `load_data()`. If it failed to load the dataset, this function returns `false`.

```cpp
assert(D.load_data(g,my_type,path.c_str()));
```

After steps above, you have a graph `g` which contains the dataset.

### Step 1 : Define the pattern

The class `Pattern` is used to store a pattern. Taking Rectangle as an example, three ways are enabled to define the pattern.

The first one is adding each edge of a pattern one by one.

```
int size = 4;
Pattern p(size);
p.add_edge(0,1);
p.add_edge(1,2);
p.add_edge(2,3);
p.add_edge(3,0);
// Rectangle is like :
// 0 -- 1
// |    |
// 3 -- 2
```

Since the Rectangle pattern is used frequently, we provide an easier way for you to load this pattern. 

```
Pattern p(PatternType::Rectangle);
```

The third way is providing an adjacency matrix in an array.

```cpp
int size = 4;
char adj_mat[17] = "0101101001011010"; // the vertex 0 has neighbor 1 and 3, so the first four character "0101" means 0 have edges to 1 and 3 but not have an edge to 0 or 2.
Pattern p(size, adj_mat);
```

### Step 2 : Define the schedule

Three things you need to decide in this step : 1) performance modeling type 2) restrictions type 3) use inclusion-exclusion optimization or not.

In class `Schedule`, you can see the constructor function :

```cpp
 Schedule(const Pattern& pattern, bool& is_pattern_valid, int performance_modeling_type, int restricts_type, bool use_in_exclusion_optimize, int v_cnt, unsigned int e_cnt, long long tri_cnt);
```

For `performance_modeling_type`, 0 means not to use any performance modeling; 1 means to use GraphPi's performance modeling.

For `restricts_type`, 0 means to use not restrictions; 1 means using GraphPi's restriction generator.

`v_cnt`, `e_cnt` and `tri_cnt`  mean the numbers of vertices, edges and triangles of the dataset respectively.

And you need to use variable `pattern` defined in the step 1.



So, if we want to use GraphPi's performance model and restrictions with inclusion-exclusion optimization, you should run like this :

```cpp
int performance_modeling_type = 1;
int restricts_type = 1;
bool use_in_exclusion_optimize = true;
bool is_pattern_valid;

Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, g->tri_cnt);
assert(is_pattern_valid);
```



### Step 3 : Run pattern matching

GraphPi is based on OpenMP to enable multi-threads, so you need to decide how many threads to use in the process of pattern matching. In our experiment, we use 24 threads since each node has 2 12-core processors.

```
int thread_num = 24;
```

Now, we can run the member function `pattern_matching` of the class `Graph`.

```
long long ans = g->pattern_matching(schedule, thread_num);
```

The result (i.e., the number of embeddings) is stored in `ans`.



### Hint

If you are still confused, you can see `tianhe/baseline_test.cpp` as an example. This program is following steps above.

## How to run the distributed version

GraphPi uses MPI to implement the distributed version.

You can see `src/run_mpi_trivial.cpp` as an example.

The parameters used in the distributed version are the same as those used in the single node version. You can choose to pass parameters through the command line or modify them directly in `src/run_mpi_trivial.cpp`.

Note that if you run more than 24 threads on a node, you need to modify the MAXTHREAD variable in the include/graphmpi.h.

## How to enable your own dataset

Some codes should be modified to enable your own dataset. Please follow the steps below.

### Step0: data format and dataset name

GraphPi only accepts input dataset of specific format: 

```
The first line should contain two integers: the number of vertices and the number of edges respectively. From the second line, each line contains two integers `x`and ` y`, representing an undirected edge `(x,y)` in the graph. The wiki-vote dataset has been placed at GraphPi/dataset/wiki-vote_input as an example.
```

And the dataset needs a name, taking `MyDataset` as an example.

### Step1: Code changes in include/dataloader.h

The name `MyDataset` should be added to `DataType`:

```c++
enum DataType {
  Patents,
  Orkut,
  ...
  MyDataset,
  Invalid
}
```

And because GraphPi's performance model needs the number of triangles in the dataset, a constant should be added (Assuming that there are 10000 triangles in MyDataset) :

```
const long long MyDataset_tri_cnt = 10000LL;
```

### Step2: Code changes in src/dataloader.cpp

The function `load_data` and `general_load_data`should be changed.

In function`load_data`, `MyDataset` need to be added to the first if condition:

```c++
if(type == Patents || type == Orkut || ... || type == MyDataset) {
  ...
}
```

And in function `general_load_data`, the switch-case statements should be modified:

```c++
switch(type) {
  case DataType::Patents : {
    g->tri_cnt = Patents_tri_cnt;
  }
  ...
  ...  
  case DataType::MyDataset : {
  	g->tri_cnt = Mydataset_tri_cnt;    
  }    
}
```

###Step3: Code changes in src/common.cpp

The function `GetDataType` should be changed.

Add a new if condition statement for `MyDataset`:

```c++
if( str == "MyDataset") {
  type = DataType::MyDataset;
}
```

After these steps, you can use `MyDataset` for pattern matching like others.



