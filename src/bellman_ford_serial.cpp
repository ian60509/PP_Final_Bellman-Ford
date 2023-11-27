#include <limits.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <omp.h>
#include <iostream>

#include "../common/graph.h"

using namespace std;

#define USE_BINARY_GRAPH 1
// #include "../common/graph.h"
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
void bellman_ford_serial(Graph g, int starter, solution *sol, int **cost){
    int num_nodes = g->num_nodes;
    int num_edges = g->num_edges;
    sol->distances = init_array_1(sol->distances , num_nodes);
    int *distances = sol->distances;

    for(int i = 0; i < num_nodes; i++){
        distances[i] = cost[starter][i];
        // printf("distances[%d] = %d \n", i, distances[i]);
    }

    distances[starter] = 0;

    for(int i = 1; i < num_nodes - 1; i++){
        for(int node = 0; node < num_nodes; node++){
            int start_edge = g->outgoing_starts[node];
            int end_edge = (node == g->num_nodes - 1)
                            ? g->num_edges
                            : g->outgoing_starts[node + 1];
            for(int edge_num = start_edge; edge_num < end_edge; edge_num++){
                Vertex outgoing_vertex = g->outgoing_edges[edge_num];
                
                if(distances[node] > distances[outgoing_vertex] + cost[outgoing_vertex][node]){
                    // printf("distances[%d] = %d, outgoing_vertex的distances[%d] = %d, cost[%d][%d] = %d\n", 
                    //     node, distances[node], outgoing_vertex, distances[outgoing_vertex], outgoing_vertex, node, cost[outgoing_vertex][node]);

                    distances[node] = distances[outgoing_vertex] + cost[outgoing_vertex][node];
                }
            }
        }
    }
}
// void floyd_warshall_serial(Graph g, solution *sol, int **cost){
//     int n = g.num_nodes;
//     // int n = 3;
//     sol->distances = init_array_2(sol->distances, n, n);
//     int **distances = sol->distances;

//     for(int i = 0; i < n; i++){
//         for(int j = 0; j < n; j++){
//             distances[i][j] = cost[i][j];
//         }
//     }

//     for(int k = 0; k < n; k++){
//         for(int i = 0; i < n; i++){
//             for(int j = 0; j < n; j++){
//                 if(distances[i][j] > distances[i][k] + distances[k][j]){
//                     // printf("好像overflow %d\n", distances[i][k] + distances[k][j]);
//                     distances[i][j] = distances[i][k] + distances[k][j];
//                 }
//             }
//         }
//     }
//     printf("dis\n");
//     print_arr_2(distances, n, n);
// }


int generateRandomPositiveInteger(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

void init_cost(int **arr, Graph g, int r, int c){
    //初始化陣列的值
    srand(time(NULL));
    for(int i = 0; i < r; i++){
        int start_edge = g->outgoing_starts[i];
        int end_edge = (i == g->num_nodes - 1)
                        ? g->num_edges
                        : g->outgoing_starts[i + 1];
        for(int node = 0; node < c; node++){
            if(i == node)
                arr[i][node] = 0;
            else{
                
                int outgoing_vertex = g->outgoing_edges[start_edge];
                int rand = generateRandomPositiveInteger(1, 15);
                if(outgoing_vertex == node ){
                    if(start_edge++ < end_edge)
                        arr[i][node] = rand;
                }else{
                    //暫時以100000代表無限大
                    arr[i][node] = 100000;
                }

            }
                
        }
    }
}

int main(int argc, char** argv){
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
    
    int **cost;
    cost = init_array_2(cost, g->num_nodes, g->num_nodes);

    init_cost(cost, g, g->num_nodes, g->num_nodes);
    // print_arr_2(cost, g->num_nodes, g->num_nodes);

    solution sol;
    
    // bellman_ford_serial(g, &sol, cost);
    bellman_ford_serial(g, 0, &sol, cost);
    cout << "---------sol.dis-----------\n";
    print_arr_1(sol.distances, g->num_nodes);
    return 0;
}