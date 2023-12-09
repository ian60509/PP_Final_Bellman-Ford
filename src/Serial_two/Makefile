GRAPH=/home/chh/pp/test/grid4x4.graph

# 如果在PP-工作站上可以使用下面這行(by Ian)
# GRAPH = /HW3/graphs/grid4x4.graph
# ../../common/graph.cpp

all: default

default: Serial.cc ../../common/graph.cpp
	g++  -o Serial $^ 

run: default
	make; ./Serial $(GRAPH)
clean:
	rm *.out 