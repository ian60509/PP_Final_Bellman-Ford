GRAPH = "/HW3/graphs/grid4x4.graph"

make clean
make
echo "-------------------start running---------------"
mpirun -n 4 ./mpi_tmp