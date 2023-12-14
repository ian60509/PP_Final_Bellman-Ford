.PHONY: all clean

CC = mpicxx

all: MPI_Bellman_Ford_Load_Balance.cpp ../../common/graph.cpp
	${CC} $^ -o MPI_BF_LB


clean:
	rm -f *.o
	rm -f MPI_BF_LB
	rm -f MPI_BF