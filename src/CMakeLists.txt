SET(TestClass
TestClass.cpp)
SET(Graph
graph.cpp)
SET(DataLoader
dataloader.cpp
)

ADD_LIBRARY(graph_mining SHARED ${DataLoader} ${Graph} ${TestClass}) 
