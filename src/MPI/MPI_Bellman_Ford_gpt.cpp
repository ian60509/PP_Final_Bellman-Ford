#include <climits>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <mpi.h>
#include <iostream>
#include <chrono>
#include "../../common/graph.h"

using namespace std;

#define USE_BINARY_GRAPH 1
#define INFINITY 1000000
struct solution {
    int *distances;
};

void print_arr_1D(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }
}

void print_arr_2D(int **arr, int r, int c) {
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            printf("arr[%d][%d] = %d\n", i, j, arr[i][j]);
        }
    }
}

int* init_array_1D(int *arr, int n) {
    arr = (int*)malloc(n * sizeof(int));
    return arr;
}

int** init_array_2D(int **arr, int r, int c) {
    // 初始化二維陣列
    arr = (int**)malloc(r * sizeof(int *));
    for (int i = 0; i < r; i++) {
        arr[i] = (int *)malloc(c * sizeof(int));
    }
    return arr;
}

void free_array_2D(int **arr, int r) {
    for (int i = 0; i < r; i++) {
        free(arr[i]);
    }
    free(arr);
}

void bellman_ford_parallel(Graph g, int starter, solution *sol, int *cost, int rank, int size) {
    auto start_time = std::chrono::high_resolution_clock::now();

    int num_nodes = g->num_nodes;
    int num_edges = g->num_edges;
    sol->distances = init_array_1D(sol->distances, num_nodes);
    int *distances = sol->distances;

    for (int i = 0; i < num_nodes; i++) {
        distances[i] = INFINITY;
    }

    MPI_Bcast(&cost[0], num_edges, MPI_INT, 0, MPI_COMM_WORLD);

    int local_start = rank * num_nodes / size;
    int local_end = (rank + 1) * num_nodes / size;

    if (rank == 0) {
        distances[starter] = 0;
    }

    for (int node = local_start; node < local_end; node++) {
        int start_edge = g->outgoing_starts[node];
        int end_edge = (node == g->num_nodes - 1) ? g->num_edges : g->outgoing_starts[node + 1];
        for (int edge_num = start_edge; edge_num < end_edge; edge_num++) {
            Vertex outgoing_vertex = g->outgoing_edges[edge_num];
            if (distances[outgoing_vertex] == INFINITY && distances[node] == INFINITY)
                continue;
            if (distances[node] + cost[edge_num] < distances[outgoing_vertex]) {
                distances[outgoing_vertex] = distances[node] + cost[edge_num];
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Gather(&distances[local_start], local_end - local_start, MPI_INT, distances, local_end - local_start, MPI_INT, 0,
               MPI_COMM_WORLD);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    if (rank == 0) {
        cout << "Bellman Ford Parallel: " << duration.count() << " microseconds" << endl;
    }
}

int generateRandomPositiveInteger(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

void init_cost_1D(int *arr, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        int rand = generateRandomPositiveInteger(1, 15);
        arr[i] = rand;
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc < 2) {
        if (rank == 0) {
            std::cerr << "Usage: " << argv[0] << " <graph>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    std::string graph_filename = argv[1];
    Graph g;
    if (USE_BINARY_GRAPH) {
        g = load_graph_binary(graph_filename.c_str());
    } else {
        g = load_graph(argv[1]);
        printf("storing binary form of graph!\n");
        store_graph_binary(graph_filename.append(".bin").c_str(), g);
        free_graph(g);
        MPI_Finalize();
        exit(1);
    }

    printf("\n");
    printf("Graph stats:\n");
    printf("  Edges: %d\n", g->num_edges);
    printf("  Nodes: %d\n", g->num_nodes);

    // cost改成一維
    int *cost;
    cost = init_array_1D(cost, g->num_edges);
    init_cost_1D(cost, g->num_edges);

    solution sol;

    bellman_ford_parallel(g, 0, &sol, cost, rank, size);

    MPI_Finalize();
    free(cost);

    return 0;
}
