#include <mpi.h>
#include <iostream>
#include <climits>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <chrono>
#include "../../common/graph.h"

using namespace std;

#define USE_BINARY_GRAPH 1
#define INFINITY 1000000
struct solution{
    int *distances;
};

void print_arr_1D(int *arr, int n);
void print_arr_2D(int **arr, int r, int c);
int* init_array_1D(int *arr, int n);
int** init_array_2D(int **arr, int r, int c);
void bellman_ford_MPI(Graph g, int starter, solution *sol, int *cost);
void bellman_ford_serial(Graph g, int starter, solution *sol, int *cost);
int generateRandomPositiveInteger(int min, int max);
void init_cost_1D(int *arr, int n);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int my_rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    Graph g = (graph*) malloc(1*sizeof(graph));
    int *cost ;
    
    if(my_rank==0){ //master
        // printf("argc=%d\n", argc);
        if(argc < 2){
            std::cerr << "Please Use This Format: " << argv[0] << " <graph>" << std::endl;
            return 1;
        }
        std::string graph_filename = argv[1];

        if (USE_BINARY_GRAPH) {
            g = load_graph_binary(graph_filename.c_str());
        } else {
            g = load_graph(argv[1]);
            printf("storing binary form of graph!\n");
            store_graph_binary(graph_filename.append(".bin").c_str(), g);
            free_graph(g);
            exit(1);
        }

    }else{ //other
    }
    
    MPI_Bcast(&(g->num_nodes), 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&(g->num_edges), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(my_rank != 0){
        // bacause other processor they are not load graph => so need to allocate memory for g's member
        g->outgoing_starts = (int*)malloc(g->num_nodes*sizeof(int));
        g->outgoing_edges = (Vertex*)malloc(g->num_edges*sizeof(Vertex));

        g->incoming_starts = (int*)malloc(g->num_nodes*sizeof(int));
        g->incoming_edges = (Vertex*)malloc(g->num_edges*sizeof(Vertex));
    }

    //every processor init their cost array => they are same
    cost = (int*)malloc(g->num_edges * sizeof(int));
    init_cost_1D(cost, g->num_edges);
    // Broadcast the graph content
    MPI_Bcast( g->outgoing_starts , g->num_nodes , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Bcast( g->outgoing_edges , g->num_edges , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Bcast( g->incoming_starts , g->num_nodes , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Bcast( g->incoming_edges , g->num_edges , MPI_INT , 0 , MPI_COMM_WORLD);
    
    


    // 完成 MPI
    MPI_Finalize();

    return 0;
}
void bellman_ford_serial(Graph g, int starter, solution *sol, int *cost){
    auto start_time = std::chrono::high_resolution_clock::now();

    int num_nodes = g->num_nodes;
    int num_edges = g->num_edges;
    sol->distances = init_array_1D(sol->distances , num_nodes);
    int *distances = sol->distances;

    for(int i = 0; i < num_nodes; i++){
        distances[i] = INFINITY;
    }

    int starter_out_edge_begin = g->outgoing_starts[starter];
    int starter_out_edge_end = g->outgoing_starts[starter+1]; //此node的end = 下一個node的start

    
    for(int edge = starter_out_edge_begin; edge < starter_out_edge_end; edge++){
        distances[g->outgoing_edges[edge]] = cost[edge];
    }
    distances[starter] = 0;

    for(int node = 1; node < num_nodes; node++){
        int start_edge = g->outgoing_starts[node];
        int end_edge = (node == g->num_nodes - 1)
                        ? g->num_edges
                        : g->outgoing_starts[node + 1];
        for(int edge_num = start_edge; edge_num < end_edge; edge_num++){
            Vertex outgoing_vertex = g->outgoing_edges[edge_num];
            if(distances[outgoing_vertex] == INFINITY && distances[node] == INFINITY)
                continue;
            if(distances[node] + cost[edge_num] < distances[outgoing_vertex] ){
                distances[outgoing_vertex] = distances[node] + cost[edge_num];
            }
        }
    }
        
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "Bellman Ford Serial: " << duration.count() << " microseconds" << endl;

}
void bellman_ford_MPI(Graph g, int starter, solution *sol, int *cost){
    auto start_time = std::chrono::high_resolution_clock::now();

    int num_nodes = g->num_nodes;
    int num_edges = g->num_edges;
    sol->distances = init_array_1D(sol->distances , num_nodes);
    int *distances = sol->distances;

    for(int i = 0; i < num_nodes; i++){
        distances[i] = INFINITY;
    }

    int starter_out_edge_begin = g->outgoing_starts[starter];
    int starter_out_edge_end = g->outgoing_starts[starter+1]; //此node的end = 下一個node的start

    
    for(int edge = starter_out_edge_begin; edge < starter_out_edge_end; edge++){
        distances[g->outgoing_edges[edge]] = cost[edge];
    }
    distances[starter] = 0;

    for(int node = 1; node < num_nodes; node++){
        int start_edge = g->outgoing_starts[node];
        int end_edge = (node == g->num_nodes - 1)
                        ? g->num_edges
                        : g->outgoing_starts[node + 1];
        for(int edge_num = start_edge; edge_num < end_edge; edge_num++){
            Vertex outgoing_vertex = g->outgoing_edges[edge_num];
            if(distances[outgoing_vertex] == INFINITY && distances[node] == INFINITY)
                continue;
            if(distances[node] + cost[edge_num] < distances[outgoing_vertex] ){
                distances[outgoing_vertex] = distances[node] + cost[edge_num];
            }
        }
    }
        
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "Bellman Ford MPI: " << duration.count() << " microseconds" << endl;

}
void print_arr_1D(int *arr, int n){
    for(int i = 0; i < n; i++){
        printf("arr[%d] = %d\n", i, arr[i]);
    }
}
void print_arr_2D(int **arr, int r, int c){
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            printf("arr[%d][%d] = %d\n", i, j, arr[i][j]);
        }
    }
}
int* init_array_1D(int *arr, int n){
    arr = (int*)malloc(n * sizeof(int));
    return arr;
}
int** init_array_2D(int **arr, int r, int c){
    //初始化二維陣列
    arr = (int**)malloc(r * sizeof(int *));
    for(int i = 0; i < r; i++){
        arr[i] = (int *)malloc(c * sizeof(int));
    }
    return arr;
}

int generateRandomPositiveInteger(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

void init_cost_1D(int *arr, int n){
    srand(time(NULL));
    for(int i = 0; i < n; i++){
        int rand = generateRandomPositiveInteger(1, 15);
        arr[i] = rand;
    }
}


