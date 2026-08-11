#include "seg.h"
#include "graph.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


void compute_max_flow(Graph *g) {
    // Algorithme d'Edmonds-Karp (Ford-Fulkerson en trouvant le chemin augmentant grâce à un parcours en largeur)

    Edge **parent = malloc((g->num_nodes + 1) * sizeof(Edge *));
    Node **file = malloc((g->num_nodes + 1) * sizeof(Node *));
    int start = 0;
    int end = 0;

    while (true) {
        //printf("Flow : %i\n", get_flow(g));
        // Parcours en largeur pour trouver le chemin augmentant

        bool reached_t = false;

        start = 0;
        end = 0;

        for (int i = 0; i < g->num_nodes; i++) {
            parent[i] = NULL;
            file[i] = NULL;
        }

        file[end++] = &g->nodes[g->source];

        while (start < end) {

            Node *node = file[start++];

            if (node == &g->nodes[g->target]) {
                reached_t = true;
                break;
            }

            for (int i_edge = 0; i_edge < node->num_edges; i_edge++) {
                Edge *edge = &node->edges[i_edge];

                if (edge->capacity - edge->flow > 1e-9 && parent[edge->to] == NULL && edge->to != g->source) {
                    file[end++] = &g->nodes[edge->to];
                    parent[edge->to] = edge;
                }
            }
        }

        if (!reached_t) {
            break;
        }

        // Recherche de la capacité résiduelle minimale sur le chemin : "goulot d'étranglement"

        int current = g->target;
        int min_res_capacity = INT_MAX;

        while (current != g->source) {
            int res_capacity = parent[current]->capacity - parent[current]->flow;

            if (res_capacity < min_res_capacity) {
                min_res_capacity = res_capacity;
            }

            current = parent[current]->rev->to;
        }

        // Modification correspondante du flot

        current = g->target;

        while (current != g->source) {
            parent[current]->flow += min_res_capacity;
            parent[current]->rev->flow -= min_res_capacity;

            current = parent[current]->rev->to;
        }
    }

    free(file);
    free(parent);
}


bool *compute_min_cut(Graph *g) {
    // On fait un parcours de graphe pour déterminer les noeuds atteignables à partir de la source sans passer par des arêtes de capacité résiduelle nulle
    // Ici j'ai fait un parcours en largeur par praticité

    bool *mask = malloc((g->num_nodes + 1) * sizeof(bool));
    Node **file = malloc((g->num_nodes + 1) * sizeof(Node *));

    int start = 0;
    int end = 0;

    for (int i = 0; i < g->num_nodes; i++) {
        mask[i] = false;
        file[i] = NULL;
    }

    mask[g->source] = true;
    file[end++] = &g->nodes[g->source];

    while (start < end) {

        Node *node = file[start++];

        for (int i_edge = 0; i_edge < node->num_edges; i_edge++) {
            Edge *edge = &node->edges[i_edge];

            if (edge->capacity != edge->flow && mask[edge->to] == false) {
                file[end++] = &g->nodes[edge->to];
                mask[edge->to] = true;
            }
        }
    }

    free(file);

    return mask;
}