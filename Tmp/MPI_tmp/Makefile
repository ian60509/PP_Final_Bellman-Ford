.PHONY: all clean

CC = mpicxx

all: mpi_tmp.cpp ../../common/graph.cpp
	${CC} $^ -o mpi_tmp

clean:
	rm -f *.o
	rm -f MPI_BF