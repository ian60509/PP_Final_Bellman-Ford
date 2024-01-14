#include <climits>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <omp.h>
#include <iostream>
#include <chrono>
#include "../../common/graph.h"

using namespace std;
#define USE_BINARY_GRAPH 1
#define DISTANCE_INFINITY 1000000

int bellman_ford_serial(Graph);

int main(int argc, char** argv){
    // ----------------- Parsing The Input, and load graph .-------------------------
    // You can check the graph I print on the terminal
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <graph>" << std::endl;
        return 1;
    }
    std::string graph_filename = argv[1];
    Graph g = (Graph)malloc(1*sizeof(graph));
    if (USE_BINARY_GRAPH) {
        g = load_graph_binary(graph_filename.c_str());
    } else {
        g = load_graph(argv[1]);
        printf("storing binary form of graph!\n");
        store_graph_binary(graph_filename.append(".bin").c_str(), g);
        free_graph(g);
        exit(1);
    }
    // print_graph(g);

    //--------------------- start running "Bellmam Ford"--------------------------
    auto start_time = std::chrono::high_resolution_clock::now();
    
    int exits_negative_cycle = bellman_ford_serial(g);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    

    //------------------------ Check  result-------------------------------
    if(exits_negative_cycle){
        printf("OMG!!! exist negative cycle!!!!!!!!!!!!!!!\n");
    }else{
        // print_distances(g, "");
    }   
    cout << "Bellman Ford Serial: " << duration.count() << " microseconds" << endl;


    return 0;
}

/*
Return Value = 0 : no negative cycle
Return Value = 1 : exists negative cycle
*/
int bellman_ford_serial(Graph g){
    
    g->distances[g->source] = 0;
    
    //--------------------------  Relax -----------------------------------
    //Iterate |V|-1 times
    for(int i=0; i<g->num_nodes-1; i++){
        bool flag = false;
        // Relax all the Edge in this graph just one time
        // We can use amortized analysis to verify this nested for loop use O(|E|)
        for(int v=0; v<g->num_nodes; v++){
            int start_edge = g->outgoing_starts[v];
            int end_edge = (v == g->num_nodes - 1)? g->num_edges: g->outgoing_starts[v + 1];

            //for each u, which is outgoing neighbor of v
            for(int edge_idx = start_edge; edge_idx < end_edge; edge_idx++){
                Vertex u = g->outgoing_edges[edge_idx];
                if( g->distances[v] == DISTANCE_INFINITY) // v can't relax ant neighbor
                    break;
                if(g->distances[v] + g->edge_cost[edge_idx] < g->distances[u] ){
                   g-> distances[u] = g->distances[v] + g->edge_cost[edge_idx];
                    flag = true;
                }
            }
            
        }
        if(!flag) break;
    }
    
    //---------------------  Check Negative Cycle---------------------------
    for(int v=0; v<g->num_nodes; v++){
        int start_edge = g->outgoing_starts[v];
        int end_edge = (v == g->num_nodes - 1)? g->num_edges: g->outgoing_starts[v + 1];

        //for each u, which is outgoing neighbor of v
        for(int edge_idx = start_edge; edge_idx < end_edge; edge_idx++){
            Vertex u = g->outgoing_edges[edge_idx];
            if( g->distances[v] == DISTANCE_INFINITY) // v can't relax ant neighbor
                break;
            if(g->distances[v] + g->edge_cost[edge_idx] < g->distances[u] ){
                return 1;
            }
        }
    }
    return 0;

}