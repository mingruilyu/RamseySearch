#include "recursion.h"

void findNeighbours(int* g, int gsize, int neighbours[][NEIGHBOR_PARAM]){
	int i,j,k;
	for(i = 0; i < NEIGHBOR_SIZE; i ++){
		neighbours[i][NEIGHBOR_PARAM-1] =100000;
	}
	for(i = 0; i < gsize; i ++) {
		for(j = i + 1; j < gsize; j ++) {
			int best_count = neighbours[NEIGHBOR_SIZE-1][NEIGHBOR_PARAM-1];
			g[i * gsize + j] = 1 - g[i * gsize + j];
			count = CliqueCountUseCache(g, gsize, i, j, best_count);
			for(k=0; k < NEIGHBOR_SIZE; k++){
				if(count < neighbours[k][NEIGHBOR_PARAM-1]){
					shift(neighbours,k);
					neighbours[k][2]=count;
					neighbours[k][0]=i;
					neighbouts[k][1]=j;
					break;					
				}
			}
			g[i * gsize + j] = 1 - g[i * gsize + j];
		}
	}
}
void shift(int neighbours[][NEIGHBOR_PARAM], int k){
	int i = NEIGHBOR_SIZE-1;
	while(i>k){
		neighbours[i][0]=neighbours[i-1][0];
		neighbours[i][1]=neighbours[i-1][1];
		neighbours[i][2]=neighbours[i-1][2];
		i--;
	}
}


int create_edge_stat( int gsize) {
	int i, j, k;
	int* node;
	int count = 0;
	for(i = 0; i < gsize; i ++) {
		for(j = 0; j < gsize; j ++)
			stat[i][j] = 0;
		people[i] = false;
	}

	for(i = 0; i < cache_7.length; i ++) {
		node = cache_7.array[i].clique_node;
		for(j = 0; j < 6; j ++) {
			for(k = j + 1; k < 7; k ++) {
				people[node[j]] = true;
				people[node[k]] = true;
				if(stat[node[j]][node[k]] == 0)
					count ++;		
				stat[node[j]][node[k]] ++;
			}
		}		
	}
	printf("number of edges: %d \n", count);
	return count;
}
