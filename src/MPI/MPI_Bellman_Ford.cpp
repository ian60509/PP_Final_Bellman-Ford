#include <mpi.h>
#include <iostream>
#include <climits>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include <chrono>
#include "../../common/graph.h"

//顏色處理
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define CYN "\e[0;36m"
#define REDB "\e[41m"

using namespace std;
#define USE_BINARY_GRAPH 1
#define DISTANCE_INFINITY 1000000



int bellman_ford_MPI(Graph);

int main(int argc, char** argv) {
    //-------------------- Init MPI --------------------
    MPI_Init(&argc, &argv);
    int my_rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Graph g = (graph*) malloc(1*sizeof(graph));
    
    //------------- get input --------------------------
    if(my_rank==0){ //master load file
        // printf("argc=%d\n", argc);
        if(argc < 2){
            std::cerr << "Please Use This Format: " << argv[0] << " <graph>" << std::endl;
            return 1;
        }
        std::string graph_filename = argv[1];

        if (USE_BINARY_GRAPH) {
            g = load_graph_binary(graph_filename.c_str());
        }else {
            g = load_graph(argv[1]);
            printf("storing binary form of graph!\n");
            store_graph_binary(graph_filename.append(".bin").c_str(), g);
            free_graph(g);
            exit(1);
        }

    }
    
    
    // ------------------------  synchronize all graph member data------------------------------------------
    MPI_Bcast(&(g->num_nodes), 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&(g->num_edges), 1, MPI_INT, 0, MPI_COMM_WORLD);

    //becasue the doesn't load graph, so have not allocate memory for g's member
    if(my_rank!=0) {
        printf("I'm %d-processor, allocacate memory....\n", my_rank);
        allocate_graph_content(g);
    }
    // Broadcast the graph content
    MPI_Bcast( g->outgoing_starts , g->num_nodes , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Bcast( g->outgoing_edges , g->num_edges , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Bcast( g->incoming_starts , g->num_nodes , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Bcast( g->incoming_edges , g->num_edges , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Bcast( g->edge_cost , g->num_edges , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Bcast( g->distances , g->num_nodes , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Bcast( &(g->source) , 1 , MPI_INT , 0 , MPI_COMM_WORLD);

    //print graph
    if(my_rank == 1) print_graph(g);

    //---------------------  Start Bellman-Ford MPI---------------------------------------
    auto start_time = std::chrono::high_resolution_clock::now();
    bellman_ford_MPI(g);
    auto end_time = std::chrono::high_resolution_clock::now();
    //---------- check result--------------
    if(my_rank==0){
        printf("I'm %d-th processors, final solution is....\n", my_rank);
        print_distances(g, "");
    }
    
    //-----------output execution time------------------
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "Bellman Ford MPI: " << duration.count() << " microseconds" << endl;
    // 完成 MPI
    MPI_Finalize();

    return 0;
}

int bellman_ford_MPI(Graph g){
    int my_rank, num_processors;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processors);
    if(g->num_nodes%num_processors!=0){
        printf("num_nodes=%d is not the multiple of number of processors=%d, that will have deadlock in MPI \n", g->num_nodes, num_processors);
        exit(0);
    }
    int stride = (g->num_nodes/num_processors);
    int *cur_distances = (int*)malloc(sizeof(int)*g->num_nodes);

    g->distances[g->source] = 0;

    //--------------------------  Relax -----------------------------------
    //Iterate |V|-1 times
    for(int i=0; i<g->num_nodes-1; i++){

        int start_v = my_rank*stride;
        int end_v = (my_rank+1)*stride;

        //Each processor is responsible for handling a subset of nodes
        for(int v=start_v; v<end_v; v++){
            int start_edge = g->outgoing_starts[v];
            int end_edge = (v == g->num_nodes - 1)? g->num_edges: g->outgoing_starts[v + 1];

            //for each u, which is outgoing neighbor of v
            for(int edge_idx = start_edge; edge_idx < end_edge; edge_idx++){
                Vertex u = g->outgoing_edges[edge_idx];
                if( g->distances[v] == DISTANCE_INFINITY) // v can't relax ant neighbor
                    break;
                if(g->distances[v] + g->edge_cost[edge_idx] < g->distances[u] ){
                   g-> distances[u] = g->distances[v] + g->edge_cost[edge_idx];
                }
            }
        }

        // Reduce  partial result of each processor in this round

        MPI_Allreduce(g->distances, cur_distances, g->num_nodes, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
        memcpy(g->distances, cur_distances, g->num_nodes*sizeof(int));
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