#include <climits>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <chrono>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include "../../common/graph.h"

#define USE_BINARY_GRAPH 1
#define DISTANCE_INFINITY 1000000

const int BLOCK_SIZE = 256;

__global__ void relaxEdges(int n, int *distances, int *mat, bool *updated) {

    int v = blockIdx.x * blockDim.x + threadIdx.x;
    int v_stride = gridDim.x * blockDim.x;

    for(int i = 0 ; i < n ; i++){
    	for(int j = v; j < n; j += v_stride){
    	    int weight = mat[i * n + j];
    	    if(weight != DISTANCE_INFINITY){
    	        int new_dist = distances[i] + weight;
    		if(new_dist < distances[j]){
    		    distances[j] = new_dist;
    		    *updated = true;
    		}
    	    }
    	}
    }
    /*
    while (v < num_nodes) {
        int start_edge = outgoing_starts[v];
        int end_edge = (v == num_nodes - 1) ? num_edges : outgoing_starts[v + 1];
        
	for(int edge_idx = start_edge; edge_idx < end_edge; edge_idx++){
            int u = outgoing_edges[edge_idx];
            int weight = edge_cost[edge_idx];
            
	    if(distances[v] == DISTANCE_INFINITY)
  	        break;

            if (distances[v] + weight < distances[u]) {
                distances[u] = distances[v] + weight;
                *updated = true;
            }
	}
	v += v_stride;
    }*/

}

int bellman_ford_cuda(int num_nodes, int *mat, Graph g, int *dis) {
    int *dev_distances, *dev_mat;
    bool *dev_updated;
    bool updated = true;
    int num_edges = g->num_edges;
    int *distances = g->distances;
    // initialize source to 0
    distances[0] = 0;

    cudaMalloc((void**)&dev_mat, num_nodes * num_nodes * sizeof(int));
    cudaMalloc((void**)&dev_distances, num_nodes * sizeof(int));
    cudaMalloc((void**)&dev_updated, sizeof(bool));

    cudaMemcpy(dev_distances, distances, num_nodes * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_mat, mat, num_nodes * num_nodes * sizeof(int), cudaMemcpyHostToDevice);

    int iteration = 0;
    
    while (updated && iteration < num_nodes - 1) {
        updated = false;
        cudaMemcpy(dev_updated, &updated, sizeof(bool), cudaMemcpyHostToDevice);

        relaxEdges<<<(num_edges + BLOCK_SIZE - 1) / BLOCK_SIZE, BLOCK_SIZE>>>(num_nodes, dev_distances, dev_mat, dev_updated);
        cudaDeviceSynchronize();

        cudaMemcpy(&updated, dev_updated, sizeof(bool), cudaMemcpyDeviceToHost);
        iteration++;
    }
    
    //std::cout<<"iterations:"<<iteration<<std::endl;

    int* distances_result = (int*)malloc(num_nodes * sizeof(int));
    cudaMemcpy(distances_result, dev_distances, num_nodes * sizeof(int), cudaMemcpyDeviceToHost);

    for (int v = 0; v < num_nodes; v++) {
        distances[v] = distances_result[v];
    }

    free(distances_result);
    cudaFree(dev_distances);
    cudaFree(dev_updated);
    cudaFree(dev_mat);
    
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

int main(int argc, char** argv) {
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
    //print_graph(g);

    // write g to an adjacent matrix
    int *mat;
    int num_nodes = g->num_nodes;
    int num_edges = g->num_edges;
    int *distances;
    distances = (int *) malloc(num_nodes * sizeof(int));
    long long int n = (long long int)num_nodes * num_nodes;
    mat = (int *) malloc(n * sizeof(int));
    
    //printf("num_nodes:%d, n:%lld\n", num_nodes, n); 
    // initialize all elements to INFINITY
    for(long long int i = 0; i < n; i++)
        mat[i] = DISTANCE_INFINITY;

    //printf("success\n");
    // for-loop all nodes for setting edge cost in mat
    for (long long int i=0; i<num_nodes; i++) {
        int start_edge = g->outgoing_starts[i];
        int end_edge = (i == num_nodes-1) ? num_edges : g->outgoing_starts[i+1];
        for (int j=start_edge; j<end_edge; j++) {
            int target = g->outgoing_edges[j];
	    long long int idx = (long long int) i * num_nodes + target;
	    //printf("%lld\n", idx);
            mat[idx] = g->edge_cost[j];
        }
    }
    
    /*
    for(long long int i = 0; i < n; i++){
        printf("%d ", mat[i]);
	if((i + 1) % num_nodes == 0)
	    printf("\n");
    }
    */
    
    //printf("success\n");

    //--------------------- start running "Bellmam Ford"--------------------------
    auto start_time = std::chrono::high_resolution_clock::now();
    
    int exits_negative_cycle = bellman_ford_cuda(num_nodes, mat, g, distances);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    //------------------------ Check  result-------------------------------
    if(exits_negative_cycle){
        printf("OMG!!! exist negative cycle!!!!!!!!!!!!!!!\n");
    }else{
        print_distances(g, "");
        /*for(int i=0; i<num_nodes; i++){
            printf("distance[%d] = %d\n", i, distances[i]);
	}*/
    }   
    std::cout << "Bellman Ford Cuda 2: " << duration.count() << " microseconds" << std::endl;
 
    return 0;
}

