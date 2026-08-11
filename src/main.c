#include "graph.h"
#include "img.h"
#include "seg.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- 1. Test de la création et destruction du graphe ---
void test_create_and_free_graph() {

    int num_nodes = 5;
    int s = 0;
    int t = 4;

    Graph *g = create_graph(num_nodes, s, t);

    // Vérification des attributs de base
    assert(g != NULL);
    assert(g->num_nodes == num_nodes);
    assert(g->s == s);
    assert(g->t == t);

    // Vérification de l'initialisation des noeuds
    for (int i = 0; i < num_nodes; i++) {
        assert(g->nodes[i].num_edges == 0);
        assert(g->nodes[i].edges != NULL);// Vérifie que le tableau d'arêtes est bien alloué
    }

    // Si free_graph a un bug (ex: int free), ça plantera ici
    free_graph(g);
}

// --- 2. Test de l'ajout des arêtes et du chaînage des pointeurs rev ---
void test_add_edge() {
    Graph *g = create_graph(3, 0, 2);

    // Ajout d'une arête de 0 vers 1 avec une capacité de 15.0
    add_edge(g, 0, 1, 15.0);

    // Vérification de l'incrémentation des compteurs
    assert(g->nodes[0].num_edges == 1);
    assert(g->nodes[1].num_edges == 1);

    // Récupération des pointeurs vers les arêtes créées
    Edge *forward = &g->nodes[0].edges[0];
    Edge *reverse = &g->nodes[1].edges[0];

    // Vérification du sens aller
    assert(forward->to == 1);
    assert(forward->capacity == 15.0);
    assert(forward->flow == 0.0);

    // Vérification du sens retour
    assert(reverse->to == 0);
    assert(reverse->capacity == 15.0);
    assert(reverse->flow == 0.0);

    // LE PLUS IMPORTANT : Vérification du chaînage croisé
    assert(forward->rev == reverse);
    assert(reverse->rev == forward);

    free_graph(g);
}

// --- 3. Test de validité du flot (Conservation, Capacité, Antisymétrie) ---
void test_check_flow() {
    Graph *g = create_graph(4, 0, 3);

    add_edge(g, 0, 1, 10.0);
    add_edge(g, 1, 2, 5.0);
    add_edge(g, 2, 3, 10.0);

    set_flow(g, 0, 1, 4.0);
    set_flow(g, 1, 2, 4.0);
    set_flow(g, 2, 3, 4.0);
    assert(check_flow(g) == true);

    set_flow(g, 1, 2, 6.0);
    assert(check_flow(g) == false);

    set_flow(g, 1, 2, 2.0);
    set_flow(g, 0, 1, 4.0);
    assert(check_flow(g) == false);

    set_flow(g, 0, 1, -15);
    set_flow(g, 1, 2, -15);
    set_flow(g, 2, 3, -15);
    assert(check_flow(g) == false);

    free_graph(g);
}

void test_compute_max_flow_get_flow() {
    // Création d'un graphe à 4 noeuds : S=0, A=1, B=2, T=3
    Graph *g = create_graph(4, 0, 3);

    // Ajout des arêtes avec des capacités spécifiques
    add_edge(g, 0, 1, 10.0);// S -> A
    add_edge(g, 0, 2, 5.0); // S -> B
    add_edge(g, 1, 2, 15.0);// A -> B (L'arête qui permet de dévier le flot)
    add_edge(g, 1, 3, 5.0); // A -> T
    add_edge(g, 2, 3, 10.0);// B -> T

    /*
     * Explication du flot théorique (Max Flow = 15) :
     * 1. Le chemin S->A->T a une capacité min de 5. (Flot = +5)
     * 2. Le chemin S->B->T a une capacité min de 5. (Flot = +5)
     * 3. Le chemin S->A->B->T utilise le reste de S->A (5) et B->T (5). (Flot = +5)
     * Total = 15.0
     */

    // Exécution de ton algorithme
    compute_max_flow(g);

    // 1. Vérification absolue de l'intégrité du réseau (capacités, symétrie, conservation)
    assert(check_flow(g) == true);

    // 2. Calcul du flot total sortant de la Source (S)
    int total_flow = get_flow(g);

    // 3. Vérification du résultat par rapport à la théorie
    assert(total_flow == 15.0);

    free_graph(g);
}

int main(int argc, char **argv) {

    printf("--- TIPE : Segmentation ---\n");

    assert(argc == 3 || argc == 4);

    const char *filename_img = argv[1];
    const char *filename_seg = argv[2];

    const double sigma = argc == 4 ? atof(argv[3]) : 20.0;

    printf("Loading image\n");

    int w;
    int h;
    unsigned char *pixels = load_pgm(filename_img, &w, &h);

    printf("Loading graph\n");

    /*
    // Objet
    int plan1[1] = {((h / 2) * w + w / 2)};
    int length1 = 1;

    // Fond
    int plan2[1] = {0};
    int length2 = 1;
    */


    // Objet : Carré de pixels au centre de l'image
    int *plan1 = malloc(400 * sizeof(int));
    int length1 = 0;
    int cx = w / 2, cy = h / 2;
    for (int dx = -10; dx < 10; dx++) {
        for (int dy = -10; dy < 10; dy++) {
            plan1[length1++] = (cy + dy) * w + (cx + dx);
        }
    }

    // Fond : Bords de l'image
    int *plan2 = malloc(2 * (w + h) * sizeof(int));
    int length2 = 0;
    for (int x = 0; x < w; x++) {
        plan2[length2++] = x;
        plan2[length2++] = (h - 1) * w + x;
    }
    for (int y = 1; y < h - 1; y++) {
        plan2[length2++] = y * w;
        plan2[length2++] = y * w + (w - 1);
    }

    Graph *g = img_to_graph(pixels, w, h, plan1, length1, plan2, length2, sigma);


    printf("Computing max flow\n");

    compute_max_flow(g);
    //print_graph(g);

    printf("Computing min cut\n");

    bool *mask = compute_min_cut(g);
    unsigned char *seg_pixels = mask_to_img(mask, w, h, 0, 254);

    save_pgm(filename_seg, seg_pixels, w, h);

    printf("Mask saved\n");

    free(plan1);
    free(plan2);
    free(seg_pixels);
    free(mask);
    free(pixels);
    free_graph(g);

    return 0;
}
