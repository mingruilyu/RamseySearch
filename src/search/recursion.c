#include "recursion.h"
#include "list.h"
#include "fifo.h"
#include "clique_count.h"

int edges[200][200];
bool vertex[200];

void findNeighbours(int* g, int gsize, int neighbours[][NEIGHBOR_PARAM]){
	int i,j,k,count;
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
					neighbours[k][1]=j;
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
			edges[i][j] = 0;
		vertex[i] = false;
	}

	for(i = 0; i < cache_7.length; i ++) {
		node = cache_7.array[i].clique_node;
		for(j = 0; j < 6; j ++) {
			for(k = j + 1; k < 7; k ++) {
				vertex[node[j]] = true;
				vertex[node[k]] = true;
				if(edges[node[j]][node[k]] == 0)
					count ++;		
				edges[node[j]][node[k]] ++;
			}
		}		
	}
	printf("number of edges: %d \n", count);
	return count;
}


int recursiveSearch(int* g, int gsize, int level, int best_ever,
                    int cur_i, int cur_j, void *taboo_list) {
    if(level == 0) return -1;
    int neighbor[NEIGHBOR_SIZE][3];
    int k, nb_i, nb_j, i, j, best_i, best_j, best_count, edge_count, count;
    int new_i, new_j;
    int to_return = -1;
    // flip current edge
    g[cur_i * gsize + cur_j] = 1 - g[cur_i * gsize + cur_j];
    CliqueCountCreateCache(g, gsize);
    //check whether this flip itself will reduce clique 7 count
    if(cache_7.length < best_ever) {
        g[cur_i * gsize + cur_j] = 1 - g[cur_i * gsize + cur_j];
        
        return cache_7.length;
    }
    // check upon this flip, is there any chance we may reduce
    // clique 7 count by flip another edge
    edge_count = create_edge_stat(gsize);
    best_count = best_ever;
    for(i = 0; i < gsize; i ++) {
        if(vertex[i]) {
            for(j = i + 1; j < gsize; j ++) {
                if(edges[i][j] != 0) {
                    g[i * gsize + j] = 1 - g[i * gsize + j];
                    count = CliqueCountUseCache(g, gsize, i, j, best_count);
                    if(count != -1 && count < best_count) {
                        best_count = count;
                        best_i = i;
                        best_j = j;
                    }
                    g[i * gsize + j] = 1 - g[i * gsize + j];
                }
            }
        }
    }
    // if nothing happened in previous loop
    // best_count would have been equal to
    // best_ever
    if(best_count < best_ever) {
        // we need to flip this edge, and leave the caller
        // of this function to flip the i, j
        g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];
        g[cur_i * gsize + cur_j] = 1 - g[cur_i * gsize + cur_j];
        FIFOInsertEdgeCount(taboo_list, best_i, best_j, best_count);
        printf("best_count = %d, edge(%d, %d) \n",
               best_count, best_i, best_j, g[best_i * gsize + best_j]);
        return best_count;
    }
    
    findNeighbours( g, gsize, neighbor);
    // recursively check neighbor one by one
    for(k = 0; k < NEIGHBOR_SIZE; k ++) {
        nb_i = neighbor[k][0];
        nb_j = neighbor[k][1];
        count = recursiveSearch(g, gsize, level - 1,
                                best_ever, nb_i, nb_j,taboo_list);
        if(count != -1) {
            to_return = count;
            FIFOInsertEdgeCount(taboo_list, nb_i, nb_j, count);
            g[nb_i * gsize + nb_j] = 1 - g[nb_i * gsize + nb_j];
            break;
        }
    }		
    g[cur_i * gsize + cur_j] = 1 - g[cur_i * gsize + cur_j];
    return to_return;	
}




