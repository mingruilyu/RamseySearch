#include <stdbool.h>
#ifndef SEARCH_H
#define SEARCH_H
#define RECURSION_THRESHOLD 70
#define SEARCH_MODE_BREADTH_FIRST true
#define SEARCH_MODE_DEPTH_FIRST false
int BFsearch(int *g, int gsize);
int DFsearch(int *g, int gsize);
int *load_graph(int* gsize);
bool compare_graph(int *g, int gsize);
extern int best_ever;
extern bool search_mode;
#endif
