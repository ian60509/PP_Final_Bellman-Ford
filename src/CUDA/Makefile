GRAPH = grid1000x1000.graph
CC = nvcc -arch=sm_75
CFLAGS = -std=c++11
LIBS = ../../common/graph.cpp

EXECUTABLE = bellman_ford_cuda

all: $(EXECUTABLE)

$(EXECUTABLE): bellman_ford_cuda.cu
	$(CC) $(CFLAGS) -o $(EXECUTABLE) bellman_ford_cuda.cu $(LIBS) -w 

.PHONY: clean

clean:
	rm -f $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE) $(GRAPH)

