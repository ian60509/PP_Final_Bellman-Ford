SERIAL_PATH_DIR=../Serial
SERIAL_PATH_EX=../Serial/serial
OPENMP_PATH_EX=./openMP
GRAPH_PATH=/HW3/graphs/grid100x100.graph



echo "--------OpenMP--------"
make
$OPENMP_PATH_EX ${GRAPH_PATH}




#graph path example
# /HW3/graphs/grid4x4.graph 
# /HW3/graphs/grid1000x1000.graph
# /HW3/graphs/grid100x100.graph
# /HW3/graphs/grid10x10.graph

