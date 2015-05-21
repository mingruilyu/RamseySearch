#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H

void PrintGraph(int *g, int gsize);
void PrintGraphCopy(int *g, int gsize, int graph_count);
int ReadGraph(char *fname, int **g, int *gsize);
void* DegradeGraph(int *old_g, int o_gsize);
void CopyGraph(int *old_g, int o_gsize, int *new_g, int n_gsize);
#endif

