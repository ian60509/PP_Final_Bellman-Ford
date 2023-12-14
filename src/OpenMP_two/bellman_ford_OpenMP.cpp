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
#define NUM_THREADS 8
#define PAD_SIZE 16 //我的本地端cache line size = 64 Byte，要做int的padding故16個
#pragma GCC optimize("O3", "Ofast")
int bellman_ford_OpenMP(Graph);

void set_distances_value(graph* g, int length, int value){
    assert(g->distances!=nullptr);
    // std::fill(g->distances, g->distances + length, value);

    #pragma omp parallel for simd num_threads(NUM_THREADS) 
    for(int i = 0; i < length; i++){
        g->distances[i] = value;
    }
}

void padding_distances(Graph g){
    free(g->distances);
    g->distances = (int*) malloc (sizeof(int)* g->num_nodes * PAD_SIZE);
    set_distances_value(g, g->num_nodes * PAD_SIZE, DISTANCE_INFINITY);
}
void print_distances(const graph* g, std::string message, int padding_size){
    assert(g->distances != nullptr);

    std::cout<<message<<std::endl;
    std::cout<<"starter="<<g->source<<std::endl;
    for(int i=0; i<g->num_nodes; i++){
        printf("distance[%d] = %d\n", i, g->distances[i*padding_size]);
    }
}
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
    padding_distances(g);
    int exits_negative_cycle = bellman_ford_OpenMP(g);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    

    //------------------------ Check  result-------------------------------
    if(exits_negative_cycle){
        printf("OMG!!! exist negative cycle!!!!!!!!!!!!!!!\n");
    }else{
        // print_distances(g, "", PAD_SIZE);
    }   
   cout << "Bellman Ford OpenMP: " << duration.count() << " microseconds" << endl;
    

    return 0;
}

/*
Return Value = 0 : no negative cycle
Return Value = 1 : exists negative cycle
*/
int bellman_ford_OpenMP(Graph g){
    g->distances[g->source * PAD_SIZE] = 0;
    
    //--------------------------  Relax -----------------------------------
    //Iterate |V|-1 times
    #pragma omp parallel for //num_threads(NUM_THREADS)
    for(int i=0; i<g->num_nodes-1; i++){
        bool flag = false;
        // printf("round-%d\n",i);
        // Relax all the Edge in this graph just one time
        // We can use amortized analysis to verify this nested for loop use O(|E|)
        // #pragma omp parallel for
        for(int v=0; v<g->num_nodes; v++){
            int start_edge = g->outgoing_starts[v];
            int end_edge = (v == g->num_nodes - 1)? g->num_edges: g->outgoing_starts[v + 1];

            //for each u, which is outgoing neighbor of v
            for(int edge_idx = start_edge; edge_idx < end_edge; edge_idx++){
                Vertex u = g->outgoing_edges[edge_idx];
                if( g->distances[v * PAD_SIZE] == DISTANCE_INFINITY) // v can't relax ant neighbor
                    break;
                
                if(g->distances[v * PAD_SIZE] + g->edge_cost[edge_idx] < g->distances[u * PAD_SIZE] ){
                    #pragma omp critical
                    g-> distances[u * PAD_SIZE] = g->distances[v * PAD_SIZE] + g->edge_cost[edge_idx];
                    
                    flag = true;
                }
                
            }
        }
        if(!flag) break;
    }
    
    //---------------------  Check Negative Cycle---------------------------
    #pragma omp parallel for
    for(int v=0; v<g->num_nodes; v++){
            int start_edge = g->outgoing_starts[v];
            int end_edge = (v == g->num_nodes - 1)? g->num_edges: g->outgoing_starts[v + 1];

            //for each u, which is outgoing neighbor of v
            for(int edge_idx = start_edge; edge_idx < end_edge; edge_idx++){
                Vertex u = g->outgoing_edges[edge_idx];
                if( g->distances[v * PAD_SIZE] == DISTANCE_INFINITY) // v can't relax ant neighbor
                    break;
                if(g->distances[v * PAD_SIZE] + g->edge_cost[edge_idx] < g->distances[u * PAD_SIZE] ){
                   return 1;
                }
            }
        }
    return 0;

}