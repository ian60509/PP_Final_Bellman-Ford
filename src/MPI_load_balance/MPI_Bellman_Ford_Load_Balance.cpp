/*
This program version uses :
    1. one master processors, assign tasks in the queue to other processors
    2. other processors:  relax the node assigned by master
*/
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
#include <queue>

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
    int worker_processors = num_processors-1;
    // if(g->num_nodes%(num_processors-1)!=0){ // use num_processors-1 bacause the num
    //     printf("num_nodes=%d is not the multiple of number of processors=%d, that will have deadlock in MPI \n", g->num_nodes, num_processors);
    //     exit(0);
    // }
    int stride = (g->num_nodes/(num_processors-1)); // because we use only num_processor -1 to run Relax edge
    int *cur_distances = (int*)malloc(sizeof(int)*g->num_nodes);

    g->distances[g->source] = 0;
    std::queue<int> frontier;
    frontier.push(g->source);


    //--------------------------  Relax -----------------------------------
    //Iterate |V|-1 times
    for(int i=0; i<g->num_nodes-1; i++){
        if(my_rank == 0){
            int unfinished_numbers = worker_processors ;
            int all_finished = 0;

            /*
                Assign node index to Worker Processes
            */

            int frontier_size = frontier.size();
            int* frontier_arr = (int*)malloc(sizeof(int) * frontier_size);
            for(int j=0; j<frontier_size; j++){
                    frontier_arr[j] = frontier.front();
                    printf("%d-th round, frontier[%d]=%d\n ",i, j, frontier_arr[j]);
                    frontier.pop();
            }
            MPI_Bcast( &frontier_size , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
            MPI_Bcast( frontier_arr, frontier_size , MPI_INT , 0 , MPI_COMM_WORLD);

            /*
                Receive Data from Other(unknown numbers)
            */
            while(unfinished_numbers>0){
                MPI_Status status;
                int recv_buf = 0;
                MPI_Recv( &recv_buf , 1 , MPI_INT ,  MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if(recv_buf>0){ //is common frontier node index
                    frontier.push(recv_buf);
                }else{ // is a finished signal
                    unfinished_numbers --;
                }
            }

            // Synchronized All Lastest Data, all processor need to call Allreduce, otherwises deadlock happened
            MPI_Allreduce(g->distances, cur_distances, g->num_nodes, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
            memcpy(g->distances, cur_distances, g->num_nodes*sizeof(int));
        } else{ //Worker Processors
            int frontier_size = 0;
            /*
                Get The Frontier & Compute Range
            */
            MPI_Bcast( &frontier_size , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
            int* frontier_arr = (int*)malloc(sizeof(int) * frontier_size);
            MPI_Bcast( frontier_arr, frontier_size , MPI_INT , 0 , MPI_COMM_WORLD);

            for(int idx=my_rank-1; idx<frontier_size; idx+=worker_processors){
                // if(my_rank==1) printf("I'm %d-processors, compute frontier node %d with index=%d\n",my_rank, frontier_arr[i], v);
                int v = frontier_arr[idx];
                int start_edge = g->outgoing_starts[v];
                int end_edge = (v == g->num_nodes - 1)? g->num_edges: g->outgoing_starts[v + 1];
                //for each u, which is outgoing neighbor of v
                for(int edge_idx = start_edge; edge_idx < end_edge; edge_idx++){
                    Vertex u = g->outgoing_edges[edge_idx];
                    if( g->distances[v] == DISTANCE_INFINITY) // v can't relax ant neighbor
                        break;
                    if(g->distances[v] + g->edge_cost[edge_idx] < g->distances[u] ){
                        g-> distances[u] = g->distances[v] + g->edge_cost[edge_idx]; //relax
                        MPI_Send( &u , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD);
                    }
                }
            }
            int finished_flag = -1;
            MPI_Send( &finished_flag , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD);
            
            /* 
                Synchronized All Lastest Data
            */
            MPI_Allreduce(g->distances, cur_distances, g->num_nodes, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
            memcpy(g->distances, cur_distances, g->num_nodes*sizeof(int));
            
        }
        
    }
    
    return 0;
}