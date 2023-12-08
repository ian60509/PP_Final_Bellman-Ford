GRAPH1 = "/HW3/graphs/grid4x4.graph"
GRAPH2 = "../../tools/simple.binary "

make clean
make
echo "-------------------start running---------------"
# mpirun -n 4 ./MPI_BF /HW3/graphs/grid4x4.graph
mpirun -n 4 ./MPI_BF /HW3/graphs/grid4x4.graph