#ifndef IMG_H
#define IMG_H

#include "graph.h"

unsigned char *load_pgm(const char *filename, int *w, int *h);
Graph *img_to_graph(unsigned char *pixels, int w, int h, int *plan1, int length1, int *plan2, int length2, double sigma);
unsigned char *mask_to_img(bool *mask, int w, int h, unsigned char color1, unsigned char color2);
void save_pgm(const char *filename, unsigned char *pixels, int w, int h);


#endif