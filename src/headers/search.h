#include <stdbool.h>
#ifndef SEARCH_H
#define SEARCH_H
#define RECURSION_THRESHOLD 50
#define SEARCH_MODE_BREADTH_FIRST true
#define SEARCH_MODE_DEPTH_FIRST false
int BFsearch(int *g, int gsize);
int DFsearch(int *g, int gsize);
int *load_graph(int* gsize);
#endif
