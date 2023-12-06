GRAPH=/home/chh/pp/test/grid4x4.graph
all:openmp

openmp: bellman_ford_openmp.cpp
	g++ -g -o openmp.out bellman_ford_openmp.cpp ../../common/graph.cpp

run:
	make; ./openmp.out $(GRAPH)
clean:
	rm *.out