#define NEIGHBOR_SIZE 4
#define NEIGHBOR_PARAM 3
#define RECURSION_DEPTH 4

typedef enum {false, true} bool;

extern int edges[200][200];
extern bool vertex[200];

void findNeighbours(int* g, int gsize, int cur_i, int cur_j, int neighbours[][NEIGHBOR_PARAM]);
void shift(int neighbours[][NEIGHBOR_PARAM], int k);
int create_edge_stat( int gsize);
int recursiveSearch(int* g, int gsize, int level, int best_ever, int cur_i, int cur_j,void *taboo_list);
