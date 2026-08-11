#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>

#define MAX_EDGES 6


typedef struct Edge {
    int to;
    int capacity;
    int flow;
    struct Edge *rev;
} Edge;

typedef struct {
    Edge *edges;
    int num_edges;
} Node;

typedef struct {
    Node *nodes;
    int num_nodes;
    int source;
    int target;
} Graph;

// Graph
Graph *create_graph(int num_nodes, int s, int t);
void add_edge(Graph *g, int from, int to, int capacity);
void free_graph(Graph *g);
void print_graph(Graph *g);

// Flow
void set_flow(Graph *g, int from, int to, int delta_flow);
bool check_flow(Graph *g);
int get_flow(Graph *g);

#endif
