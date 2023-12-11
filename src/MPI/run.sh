#!/bin/bash

graph_1 = "/HW3/graphs/grid4x4.graph"
graph_2 = "../../tools/simple.binary "

make clean
make
echo "-------------------start running---------------"
# mpirun -n 4 ./MPI_BF /HW3/graphs/grid4x4.graph

echo "----------------- start running in varios host-----------------------"
parallel-scp -h ./host_file/host.txt -r ~/PP_Final_Bellman-Ford/src/MPI ~/PP_Final_Bellman-Ford/src/MPI
mpirun -np 4 --hostfile ./host_file/host.txt  MPI_BF /HW3/graphs/grid10x10.graph
# mpirun -n 2 ./MPI_BF ../../tools/binary_file/simple2.binary ``