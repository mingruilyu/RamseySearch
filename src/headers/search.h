#include <stdbool.h>
#ifndef SEARCH_H
#define SEARCH_H
int search(int *g, int gsize, int new_graph_count, bool* recv_flag);
int *load_graph(int* gsize);
#endif
