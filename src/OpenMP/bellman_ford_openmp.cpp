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
#define INFINITY 1000000
struct solution{
    int *distances;
};
void print_arr_1(int *arr, int n){
    for(int i = 0; i < n; i++){
        printf("arr[%d] = %d\n", i, arr[i]);
    }
}
void print_arr_2(int **arr, int r, int c){
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            printf("arr[%d][%d] = %d\n", i, j, arr[i][j]);
        }
    }
}
int* init_array_1(int *arr, int n){
    arr = (int*)malloc(n * sizeof(int));
    return arr;
}
int** init_array_2(int **arr, int r, int c){
    //初始化二維陣列
    arr = (int**)malloc(r * sizeof(int *));
    for(int i = 0; i < r; i++){
        arr[i] = (int *)malloc(c * sizeof(int));
    }
    return arr;
}
void bellman_ford_openmp(Graph g, int starter, solution *sol, int *cost){
    auto start_time = std::chrono::high_resolution_clock::now();
    int num_nodes = g->num_nodes;
    int num_edges = g->num_edges;
    sol->distances = init_array_1(sol->distances , num_nodes);
    int *distances = sol->distances;

    #pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < num_nodes; i++){
        distances[i] = INFINITY;
    }

    #pragma omp parallel for schedule(dynamic)
    int starter_out_edge_begin = g->outgoing_starts[0];
    int starter_out_edge_end = g->outgoing_starts[1];
    for(int edge = starter_out_edge_begin; edge < starter_out_edge_end; edge++){
        distances[g->outgoing_edges[edge]] = cost[edge];
    }
    distances[starter] = 0;

    #pragma omp parallel for
    for(int node = 1; node < num_nodes; node++){
        int start_edge = g->outgoing_starts[node];
        int end_edge = (node == g->num_nodes - 1)
                        ? g->num_edges
                        : g->outgoing_starts[node + 1];
        #pragma omp parallel for
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
    cout << "Bellman Ford OpenMP: " << duration.count() << " microseconds" << endl;

}

int generateRandomPositiveInteger(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

void init_cost_1(int *arr, int n){
    srand(time(NULL));
    for(int i = 0; i < n; i++){
        int rand = generateRandomPositiveInteger(1, 15);
        arr[i] = rand;
    }
}

int main(int argc, char** argv){
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <graph>" << std::endl;
        return 1;  // 返回非零值表示錯誤
    }
    std::string graph_filename = argv[1];
    std::cout << argv[1] << std::endl;
    Graph g;
    if (USE_BINARY_GRAPH) {
        g = load_graph_binary(graph_filename.c_str());
    } else {
        g = load_graph(argv[1]);
        printf("storing binary form of graph!\n");
        store_graph_binary(graph_filename.append(".bin").c_str(), g);
        free_graph(g);
        exit(1);
    }
    printf("\n");
    printf("Graph stats:\n");
    printf("  Edges: %d\n", g->num_edges);
    printf("  Nodes: %d\n", g->num_nodes);
    
    int *cost;
    cost = init_array_1(cost, g->num_edges);
    init_cost_1(cost, g->num_edges);

    solution sol;
    
    // bellman_ford_openmp(g, &sol, cost);
    bellman_ford_openmp(g, 0, &sol, cost);
    free(cost);

    // cout << "---------sol.dis-----------\n";
    // print_arr_1(sol.distances, g->num_nodes);
    return 0;
}