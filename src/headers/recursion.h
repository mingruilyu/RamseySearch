#define NEIGHBOR_SIZE 10
#define NEIGHBOR_PARAM 3


void findNeighbours(int* g, int gsize, int neighbours[][NEIGHBOR_PARAM]);
void shift(int neighbours[][NEIGHBOR_PARAM], int k);
int create_edge_stat( int gsize);
