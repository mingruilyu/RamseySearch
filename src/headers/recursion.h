#define NEIGHBOR_SIZE 5
#define NEIGHBOR_PARAM 3
#define RECURSION_DEPTH 7

typedef enum {false, true} bool;

extern int edges[200][200];
extern bool vertex[200];

int findNeighbours(int* g, int gsize, int cur_i, int cur_j, int neighbours[][NEIGHBOR_PARAM],void *taboo_list);
void shift(int neighbours[][NEIGHBOR_PARAM], int k);
int create_edge_stat( int gsize);
int recursiveSearch(int* g, int gsize, int level, int best_ever, int cur_i, int cur_j,void *taboo_list);