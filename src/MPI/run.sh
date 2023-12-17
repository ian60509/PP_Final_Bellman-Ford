#!/bin/bash

graph_1 = "/HW3/graphs/grid4x4.graph"
graph_2 = "../../tools/simple.binary "

make clean
make
echo "-------------------start running on localhost---------------"
mpiexec -n 4 MPI_BF /HW3/graphs/tiny.graph

# echo "----------------- start running in varios host-----------------------"
# parallel-scp -h ./host_file/host.txt -r ~/PP_Final_Bellman-Ford/src/MPI ~/PP_Final_Bellman-Ford/src
# mpirun -np 4 --hostfile ./host_file/host.txt  MPI_BF /HW3/graphs/grid100x100.graph


# /HW3/graphs/*.graph
# tiny
# grid100x100.graph
# soc-epinions1_500k
# ego-twitter_2m
# com-youtube_3m
# random_10m
# soc-pokec_30m
# soc-livejournal1_68m 
# com-orkut_117m 
# rmat_200m 
# random_500m


# ../../tools/binary_file/sparse1K.binary
# ../../tools/binary_file/dense_1K.binary