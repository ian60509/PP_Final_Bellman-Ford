#ifndef __GRAPH_H__
#define __GRAPH_H__

using Vertex = int;

struct graph
{
    // Number of edges in the graph
    int num_edges;
    // Number of vertices in the graph
    int num_nodes;

    // The node reached by vertex i's first outgoing edge is given by
    // outgoing_edges[outgoing_starts[i]].  To iterate over all
    // outgoing edges, please see the top-down bfs implementation.
    int* outgoing_starts;                // |V| member
    Vertex* outgoing_edges;              //|E| member

    int* incoming_starts;                // |V| member
    Vertex* incoming_edges;              //|E| member

    int* edge_cost; // cost 1D array, 1-1 mapping to outgoing edges
    int* distances;                      // |V| member
    
    int source;
};

using Graph = graph*;

/* Getters */
static inline int num_nodes(const Graph);
static inline int num_edges(const Graph);

static inline const Vertex* outgoing_begin(const Graph, Vertex);
static inline const Vertex* outgoing_end(const Graph, Vertex);
static inline int outgoing_size(const Graph, Vertex);

static inline const Vertex* incoming_begin(const Graph, Vertex);
static inline const Vertex* incoming_end(const Graph, Vertex);
static inline int incoming_size(const Graph, Vertex);


/* IO */
Graph load_graph(const char* filename);
Graph load_graph_binary(const char* filename);
void store_graph_binary(const char* filename, Graph);

void print_graph(const graph*);
void print_distances(const graph*, std::string);

/* Modify Graph*/
void allocate_graph_content(graph* g);
void set_distances_value(graph* g, int value);

/* Deallocation */
void free_graph(Graph);


/* Included here to enable inlining. Don't look. */
#include "graph_internal.h"

#endif
