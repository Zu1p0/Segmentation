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
void free_graph(Graph *g);
void print_graph(Graph *g);
void add_edge(Graph *g, int from, int to, int capacity);
static inline Node *get_node(Graph *g, int i_node) { return &g->nodes[i_node]; }
static inline Edge *get_edge(Node *node, int i_edge) { return &node->edges[i_edge]; }

// Flow
void set_flow(Graph *g, int from, int to, int delta_flow);
bool check_flow(Graph *g);
int get_flow(Graph *g);
static inline void push_flow(Edge *e, int flow) {
    e->flow += flow;
    e->rev->flow -= flow;
}
static inline int residual_capacity(Edge *edge) { return edge->capacity - edge->flow; }

#endif
