GRAPH=/home/chh/pp/test/grid4x4.graph

# 如果在PP-工作站上可以使用下面這行(by Ian)
# GRAPH = /HW3/graphs/grid4x4.graph

all: default

default: bellman_ford_serial.cpp
	g++ -g -o serial.out bellman_ford_serial.cpp ../../common/graph.cpp

run: default
	make; ./serial.out $(GRAPH)
clean:
	rm *.out 