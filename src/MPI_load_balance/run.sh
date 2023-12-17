#!/bin/bash

graph_1 = "/HW3/graphs/grid4x4.graph"
graph_2 = "../../tools/simple.binary "

make clean
make
echo "-------------------start running---------------"
mpirun -n 3 ./MPI_BF_LB /HW3/graphs/grid4x4.graph

echo "----------------- start running in varios host-----------------------"
parallel-scp -h ./host_file/host.txt -r ~/PP_Final_Bellman-Ford/src/MPI_Load_Balance ~/PP_Final_Bellman-Ford/src
mpirun -np 4 --hostfile ./host_file/host.txt  MPI_BF_LB /HW3/graphs/grid100x100.graph
# mpirun -n 2 ./MPI_BF ../../tools/binary_file/simple2.binary ``