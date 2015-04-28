#include "clique_count.h"
#include "graph.h"
#include "stdio.h"
int main() {
	int* g;
	int gsize;
	ReadGraph("ce", &g, &gsize);
	PrintGraph(g, gsize);
	
	CliqueCount(g, gsize);
	List *cache_6 = list_init(6);
	List *cache_7 = list_init(7);

	CliqueCountCreateCache(g, gsize, cache_6, cache_7);
	printf("# of clique_6:\t%d\n", cache_6->length);
	printf("# of clique_7:\t%d\n", cache_7->length);
	
	int i = 40, j = 80;

	printf("Flipping (%d, %d)\n", i, j);
	g[i * gsize + j] = 1 - g[i * gsize + j];
	int count = CliqueCount(g, gsize);
	printf("# of clique_7 by normal:\t%d\n", count);
	count = CliqueCountUseCache(g, gsize, i, j, 10000, cache_6, cache_7);
	printf("# of clique_7 by cache:\t%d\n", count);
	free(g);

	return 0;
}
