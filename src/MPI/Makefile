.PHONY: all clean

CC = mpicxx

all: MPI_Bellman_Ford.cpp ../../common/graph.cpp
	${CC} $^ -o MPI_BF

clean:
	rm -f *.o
	rm -f MPI_BF