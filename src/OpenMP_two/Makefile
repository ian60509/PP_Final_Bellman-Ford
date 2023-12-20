GRAPH=/home/chh/pp/test/grid4x4.graph
all:openMP

openMP: bellman_ford_OpenMP.cpp ../../common/graph.cpp
	g++ -g -o openMP -fopenmp $^

run:
	make; ./openmp.out $(GRAPH)
clean:
	rm *.out