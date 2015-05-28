#include <stdbool.h>
#ifndef SEARCH_H
#define SEARCH_H
#define RECURSION_THRESHOLD 50
int search(int *g, int gsize);
int *load_graph(int* gsize);
#endif
