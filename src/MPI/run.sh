#!/bin/bash

graph_1 = "/HW3/graphs/grid4x4.graph"
graph_2 = "../../tools/simple.binary "

make clean
make
echo "-------------------start running---------------"
mpirun -n 4 ./MPI_BF /HW3/graphs/grid4x4.graph
# mpirun -n 2 ./MPI_BF ../../tools/binary_file/simple2.binary ``