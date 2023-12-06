GRAPH=/home/chh/pp/test/grid4x4.graph
all: default

default: bellman_ford_serial.cpp
	g++ -g -o serial.out bellman_ford_serial.cpp ../../common/graph.cpp

run: default
	make; ./serial.out $(GRAPH)
clean:
	rm *.out