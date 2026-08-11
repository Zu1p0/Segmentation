#include "graph.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Graph

Graph *create_graph(int num_nodes, int source, int target) {
    assert(num_nodes >= 2);
    Graph *g = malloc(sizeof(Graph));

    g->num_nodes = num_nodes;
    g->source = source;
    g->target = target;

    g->nodes = malloc(num_nodes * sizeof(Node));

    for (int i = 0; i < num_nodes; i++) {
        g->nodes[i].num_edges = 0;
        if (i == g->source || i == g->target) {
            g->nodes[i].edges = malloc(g->num_nodes * sizeof(Edge));
        } else {
            g->nodes[i].edges = malloc(MAX_EDGES * sizeof(Edge));
        }
    }

    return g;
}

void add_edge(Graph *g, int from, int to, int capacity) {
    assert(g->nodes[from].num_edges < MAX_EDGES || from == g->source || from == g->target);
    assert(g->nodes[to].num_edges < MAX_EDGES || to == g->source || to == g->target);

    Edge *dir = &(g->nodes[from].edges[g->nodes[from].num_edges]);
    Edge *rev = &(g->nodes[to].edges[g->nodes[to].num_edges]);

    dir->to = to;
    dir->capacity = capacity;
    dir->flow = 0.;
    dir->rev = rev;

    rev->to = from;
    rev->capacity = capacity;
    rev->flow = 0.;
    rev->rev = dir;

    g->nodes[from].num_edges++;
    g->nodes[to].num_edges++;
}

void free_graph(Graph *g) {
    for (int i = 0; i < g->num_nodes; i++) {
        free(g->nodes[i].edges);
    }
    free(g->nodes);
    free(g);
}

void print_graph(Graph *g) {
    if (g == NULL) {
        printf("Erreur : Le graphe est NULL.\n");
        return;
    }

    printf("\n");
    printf("==============================================================\n");
    printf("                    GRAPH REPRESENTATION                      \n");
    printf("==============================================================\n");

    for (int i = 0; i < g->num_nodes; i++) {
        Node *node = &g->nodes[i];

        // Affichage de l'en-tête du noeud avec un tag spécial pour S et T
        if (i == g->source) {
            printf("\n[ Node %3d ] (s)\n", i);
        } else if (i == g->target) {
            printf("\n[ Node %3d ] (t)\n", i);
        } else {
            printf("\n[ Node %3d ]\n", i);
        }

        // Si le noeud n'a aucune arête
        if (node->num_edges == 0) {
            printf("  └── (No connections)\n");
            continue;
        }

        // Parcours et affichage de toutes les arêtes du noeud
        for (int j = 0; j < node->num_edges; j++) {
            Edge *edge = &node->edges[j];

            // Détermine si c'est la dernière arête pour fermer le dessin proprement
            const char *prefix = (j == node->num_edges - 1) ? "└──►" : "├──►";

            // Vérifie si l'arête est saturée (flot == capacité et capacité > 0)
            int is_saturated = (edge->capacity != 0 && edge->flow == edge->capacity);
            int is_saturated_back = (edge->capacity != 0 && edge->flow == -edge->capacity);

            // Affichage formaté (les %6.2f permettent d'aligner les nombres décimaux)
            printf("  %s Node %3d   [ Flow: %4i / Cap: %4i ] %s %s\n",
                   prefix,
                   edge->to,
                   edge->flow,
                   edge->capacity,
                   is_saturated ? " [*SATURATED*]" : "",
                   is_saturated_back ? " [*SATURATED (reverse)*]" : "");
        }
    }
    printf("==============================================================\n\n");
    printf(" Total Nodes: %d  |  s : %d  |  t : %d | Flow : %i\n",
           g->num_nodes, g->source, g->target, get_flow(g));
    printf("==============================================================\n\n");
}

// Flow

void set_flow(Graph *g, int from, int to, int flow) {
    for (int i_edge = 0; i_edge < g->nodes[from].num_edges; i_edge++) {
        Edge *edge = &g->nodes[from].edges[i_edge];
        if (edge->to == to) {
            edge->flow = flow;
            edge->rev->flow = -flow;
            break;
        }
    }
}

/*
void add_flow(Graph *g, int from, int to, int delta_flow) {
    for (int i_edge = 0; i_edge < g->nodes[from].num_edges; i_edge++) {
        Edge *edge = &g->nodes[from].edges[i_edge];
        if (edge->to == to) {
            edge->flow += delta_flow;
            edge->rev->flow -= delta_flow;
            break;
        }
    }
}
*/

bool check_flow(Graph *g) {
    for (int i_node = 0; i_node < g->num_nodes; i_node++) {

        Node *node = &g->nodes[i_node];

        int n_flow = 0.;

        for (int i_edge = 0; i_edge < node->num_edges; i_edge++) {
            Edge *edge = &node->edges[i_edge];

            if (edge->flow + edge->rev->flow != 0) {
                // printf("L'anti-symetrie n'est pas respectée entre %i et %i\n", i_node, edge->to);
                return false;
            }

            if (edge->flow > edge->capacity) {
                // printf("Le flot de %i à %i dépasse la capacité %i\n", i_node, edge->to, edge->capacity);
                return false;
            }

            n_flow += edge->flow;
            // printf("Ajouté %i à %i\n", edge.flow, i_node);
        }

        // printf("Netflow de %i = %i\n", i_node, flow);
        if (i_node != g->source && i_node != g->target && n_flow != 0) {
            return false;
        }
    }

    return true;
}

int get_flow(Graph *g) {
    Node *source = &g->nodes[g->source];
    int flow = 0.;

    for (int i_edge = 0; i_edge < source->num_edges; i_edge++) {
        flow += source->edges[i_edge].flow;
    }

    return flow;
}