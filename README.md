# GraphPi

GraphPi is being refactored. The official version will be released in the final version of our paper.



## How to run on sigle machine

In this section, you will learn how to run GraphPi on single machine with multi threads.(using OpenMP)



### Step 0 : Load the dataset

The class `DataLoader` is used to load the dataset.  And the dataset will store in class `Graph` .

The format of the dataset may be a little different from its download version. The first line should contain two integer, the number of vertices and the number of edges respectively. From the second line, each line contains two integer `x`and ` y`,representing an edge `(x,y)` in the graph. Since GraphPi's performan model needs the number of triangles in the dataset, a constant variable named "$dataset_tri_cnt" is needed in `/include/dataloader.h`(See code for more detail).

We have already prepared these clasical datasets for GraphPi system: `Patents` ,`Orkut` ,`LiveJournal`, `MiCo`, `Twitter`, `CiteSeer` and `WikiVote` . We only upload `WikiVote` dataset to github as an example, because some datasets can be very large.

When the dataset is prepared, we can load the dataset easily. First,  you need to define a pointer of graph and a dataloader.

```cpp
Graph *g;
Dataloader D;
```

Then, you need to use `std::string` to assign the dataset's name and file path.

```cpp
const std::string type_str = "Wiki-Vote";
const std::string path = "./datasets/wiki-vote_input"
```

In `/include/common.h`, we provide many function to make your coding more easy. Using `GetDataType` to transform `std::string` to `DataType`.

```cpp
DataType my_type;
GetDataType(my_type, type_str);
```

Now you can load the dataset using the function `load_data()`. If it failed to load the dataset, this function will return `false`.

```cpp
assert(D.load_data(g,my_type,path.c_str()));
```

After steps above, you have a graph `g` which containing Wiki-Vote dataset.

### Step 1 : Define the pattern

The class `Pattern` is used to store the pattern. There are several ways to define a pattern.

Taking rectangle as an example, three ways are enabled to define the pattern.

The first one is adding edges yourself.

```
int size = 4;
Pattern p(size);
p.add_edge(0,1);
p.add_edge(1,2);
p.add_edge(2,3);
p.add_edge(3,0);
// the pattern rectangle is like :
// 0 -- 1
// |    |
// 3 -- 2
```

Since the rectangle was used frequently, we provide an easier way for you to load the rectangle. 

```
Pattern p(PatternType::Rectangle);
```

And the third way is a little bit brute-force, you can provide adjoint matrix as a char array.

```cpp
int size = 4;
char adj_mat[17] = "0101101001011010"; // the vertex 0 has neighbor 1 and 3, so the first four character "0101" means 0 have edges to 1 and 3 but not have edge to 0 or 2.
Pattern p(size, adj_mat);
```

###Step 2 : Define the schedule

Three things you need to decide in this step : 1) performance modeling type 2) restricts type 3) the use of inclusion-exclusion optimize.

In class `Schedule` you can see the constructor function :

```cpp
 Schedule(const Pattern& pattern, bool& is_pattern_valid, int performance_modeling_type, int restricts_type, bool use_in_exclusion_optimize, int v_cnt, unsigned int e_cnt, long long tri_cnt);
```

For `performance_modeling_type`, 0 means not use performance modeling; 1 means using GraphPi's.

For `restricts_type`, 0 means not use restricts; 1 means using GraphPi's.

`v_cnt` means number of vertices, `e_cnt` means number of edges, `tri_cnt` means numnber of triangles.

And you need to use variable `pattern` defined in the step 1.



So if we want to use GraphPi's performance model and restricts without inclusion-exclusion optimize, you should run like this :

```cpp
int performance_modeling_type = 1;
int restricts_type = 1;
bool use_in_exclusion_optimize = false;
bool is_pattern_valid;

Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize, g->v_cnt, g->e_cnt, g->tri_cnt);
assert(is_pattern_valid);
```



###Step 3 : Run pattern matching

GraphPi is based on Openmp to enable multi-threads, so you need to decide how many threads you will use in the process of pattern matching. In our experiment, we use 24 threads.

```
int thread_num = 24;
```

Now, we can run the member function `pattern_matching` of class Graph.

```
long long ans = g->pattern_matching(schedule, thread_num);
```

And the result is store in variable `ans`（short for answer).



### Hint

If you are still confused, you can see `tianhe/baseline_test.cpp` as an example. This program is following steps above.

Many function are implemented for debug and experiment, you can just ignored them.