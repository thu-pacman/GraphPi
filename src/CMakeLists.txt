SET(GraphMiningSrc
TestClass.cpp
graph.cpp
dataloader.cpp
pattern.cpp
prefix.cpp
schedule.cpp
vertex_set.cpp
)

ADD_LIBRARY(graph_mining SHARED ${GraphMiningSrc}) 
