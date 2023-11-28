GRAPH_TOOLS_NAME=graphTools
PRODUCE_TEXT = produce_text

main:
	g++ -std=c++11 -g -O3 -o ${GRAPH_TOOLS_NAME} graphTools.cpp ../common/graph.cpp

PT:
	g++ -std=c++11 -g -O3 -o ${PRODUCE_TEXT} ./ProduceText.cpp 
clean:
	rm -rf pr *~ *.*~ ${GRAPH_TOOLS_NAME}
	rm -r ${PRODUCE_TEXT}

test:
	make -C ../src/Serial