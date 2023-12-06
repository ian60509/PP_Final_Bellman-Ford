SERIAL_PATH_DIR=../Serial
SERIAL_PATH_EX=../Serial/serial.out
OPENMP_PATH_EX=./openmp.out
# GRAPH=/home/chh/pp/test/grid4x4.graph
echo "type in <Graph path>"
read GRAPHINPUT
echo "--------serial--------"
make
$SERIAL_PATH_EX $GRAPHINPUT
# make run -C ../Serial
# $(SERIAL_PATH)
# ./serial.out $(GRAPH)

echo "--------OpenMP--------"
make
$OPENMP_PATH_EX $GRAPHINPUT
# make run
# ./openmp.out $(GRAPH)