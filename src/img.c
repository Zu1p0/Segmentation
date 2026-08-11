#include "img.h"
#include "graph.h"
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"

void skip_comments(FILE *fp) {
    int ch;
    char line[256];
    while ((ch = fgetc(fp)) != EOF && isspace(ch));
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        skip_comments(fp);
    } else {
        fseek(fp, -1, SEEK_CUR);
    }
}

unsigned char *load_pgm(const char *filename, int *w, int *h) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Erreur: Impossible d'ouvrir %s\n", filename);
        exit(1);
    }
    char magic[3];
    fscanf(f, "%2s", magic);
    if (strcmp(magic, "P5") != 0) {
        printf("Erreur: Format non supporté (utilise du PGM binaire P5)\n");
        exit(1);
    }

    skip_comments(f);
    fscanf(f, "%d %d", w, h);
    skip_comments(f);
    int maxval;
    fscanf(f, "%d", &maxval);
    fgetc(f);

    unsigned char *pixels = malloc((*w) * (*h));
    fread(pixels, 1, (*w) * (*h), f);
    fclose(f);
    return pixels;
}

#pragma GCC diagnostic pop

int capacity(unsigned char a, unsigned char b, double sigma) {
    int diff = abs(a - b);

    return (int) (exp(-((double) (diff * diff)) / (2.0 * sigma * sigma)) * 1e4);
}


Graph *img_to_graph(unsigned char *pixels, int w, int h, int *plan1, int length1, int *plan2, int length2, double sigma) {
    Graph *g = create_graph(w * h + 2, w * h, w * h + 1);
    for (int i = 0; i < w * h; i++) {
        if (i < (h - 1) * w) {// Arête en bas
            add_edge(g, i, i + w, capacity(pixels[i], pixels[i + w], sigma));
        }
        if ((i + 1) % w != 0) {// Arête à droite
            add_edge(g, i, i + 1, capacity(pixels[i], pixels[i + 1], sigma));
        }
    }

    // Plan 1
    for (int i = 0; i < length1; i++) {
        add_edge(g, g->s, plan1[i], 1e6);
    }

    // Plan 2

    for (int i = 0; i < length2; i++) {
        add_edge(g, plan2[i], g->t, 1e6);
    }

    return g;
}

unsigned char *mask_to_img(bool *mask, int w, int h, unsigned char color1, unsigned char color2) {
    unsigned char *pixels = malloc(w * h * sizeof(unsigned char));

    for (int i = 0; i < w * h; i++) {
        pixels[i] = mask[i] ? color1 : color2;
    }

    return pixels;
}

void save_pgm(const char *filename, unsigned char *pixels, int w, int h) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Erreur: Impossible d'écrire %s\n", filename);
        exit(1);
    }
    fprintf(f, "P5\n%d %d\n255\n", w, h);
    fwrite(pixels, 1, w * h, f);
    fclose(f);
}