#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "list.h"
#include "fifo.h"	/* for taboo list */
#include "time.h"
#include "clique_count.h"
#include "recursion.h"
#define MAXSIZE (541)
#define TABOOSIZE (250)
#define BIGCOUNT (1000)
#define THRESHOLD (10)
#define NEIGHBOR_SIZE (10)
#define RECURSION_DEPTH (20)
/*
 * example of very simple search for R(7, 7) counter 
 * examples starts with a small randomized graph and 
 * works its way up to successively larger graphs one 
 * at a time
 * uses a taboo list of size #TABOOSIZE# to hold and 
 * encoding of and edge (i, j) + clique_count
 */
typedef enum {false, true} bool;

int main(int argc, char *argv[])
{
	int *g, *new_g;
	int gsize, count, i, j, k, best_count, 
			best_i, best_j;
	void *taboo_list;
	srand(time(NULL));
	bool backtrack_flag = false, break_flag = false;
	best_count = BIGCOUNT;
	if(argc == 1) { 
		// start with graph of size 8
		gsize = 8;
		g = (int *) malloc(gsize * gsize * sizeof(int));
		if(g == NULL) exit(1);
		new_g = g;
		// start out with a random matrix
		for(i = 0; i < gsize; i ++) {
			for(j = i + 1; j < gsize; j ++) {
				rand_no = rand() % 100;
				if(rand_no > 50) g[i * gsize + j] = 1; 
			
				else new_g[i * gsize + j] = 0; 
			}
		}
	} else {
		ReadGraph("../../ce", &g, &gsize);
		PrintGraph(g, gsize);
		CliqueCountCreateCache(g, gsize);
		best_count = BIGCOUNT;
/*		new_g = (int *) malloc((gsize + 1) 
															* (gsize + 1) * sizeof(int));
		if(new_g == NULL) exit(1);
		// copy the old graph into the new graph leaving 
		// the last row and last column alone
		CopyGraph(g, gsize, new_g, gsize + 1);
		// throw away the old graph and make new one 
		free(g);
		g = new_g;
		// randomly assigned value for last column
		for(i = 0; i < gsize + 1; i ++) {
			rand_no = rand() % 100;
			if(rand_no > 50) {
				new_g[i * (gsize + 1) + gsize] = 1; // last column
				new_g[gsize * (gsize + 1) + i] = 1;	
			}
			else {
				new_g[i * (gsize + 1) + gsize] = 0; // last column
				new_g[gsize * (gsize + 1) + i] = 0;	
			}
		}
		gsize = gsize + 1;
			// reset the taboo list for the new graph*/
	}
	// make a fifo to use as the taboo list
	taboo_list = FIFOInitEdge(TABOOSIZE);

	if(taboo_list == NULL) exit(1);
	// find a start point
	best_count = BIGCOUNT;
	for(i = 0; i < gsize; i ++) {
		for(j = i + 1; j < gsize; j ++) {
   		g[i * gsize + j] = 1 - g[i * gsize + j];
			count = CliqueCountUseCache(g, gsize, i, j, best_count);
			if(count != -1 && !FIFOFindEdgeCount(taboo_list, i, j, count)) {
      	if(count < best_count) {
          best_count = count;
          best_i = i;
          best_j = j;
				}
			}
      g[i * gsize + j] = 1 - g[i * gsize + j];
		}
	}
		
	while(gsize < 206) {
		CliqueCountCreateCache(g, gsize);
		best_count = cache_7.length;
		while(best_count != 0) {
			count = recursiveSearch(g, gsize, RECURSION_DEPTH, best_count, 
															best_i, best_j)
			if(count != -1) {
				printf("EVOLVNG!!!!!!!!!!\n");
				PrintGraph(g, gsize);
				FIFOInsertEdgeCount(taboo_list, new_i, new_j, best_count);
				printf("best_count = %d, edge(%d, %d) \n", 
							 best_count, new_i, new_j, g[new_i * gsize + new_j]);
			} else {
				CliqueCountCreateCache(g, gsize);
				for(k = 0; k < RECURSION_DEPTH; k ++) {
					best_count = BIGCOUNT;
					for(i = 0; i < gsize; i ++) {
						for(j = i + 1; j < gsize; j ++) {
          		g[i * gsize + j] = 1 - g[i * gsize + j];
							count = CliqueCountUseCache(g, gsize, i, j, best_count);
							if(count != -1 && !FIFOFindEdgeCount(taboo_list, i, j, count)) {
            		if(count < best_count) {
              		best_count = count;
              		best_i = i;
              		best_j = j;
								}
							}
          		g[i * gsize + j] = 1 - g[i * gsize + j];
						}
					} 
					g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];
					FIFOInsertEdgeCount(taboo_list, best_i, best_j, best_count);
				}
				PrintGraph(g, gsize);
			}
		}

		// if we have a counter example.
		if(!backtrack_flag)	{
			printf("Eureka! Counter-example found!\n");
			PrintGraph(g, gsize);
			// make a new graph one size bigger
			new_g = (int *) malloc((gsize + 1) 
															* (gsize + 1) * sizeof(int));
			if(new_g == NULL) exit(1);
			// copy the old graph into the new graph leaving 
			// the last row and last column alone
			CopyGraph(g, gsize, new_g, gsize + 1);
			// throw away the old graph and make new one 
			free(g);
			g = new_g;
			// randomly assigned value for last column
			for(i = 0; i < gsize + 1; i ++) {
				rand_no = rand() % 100;
				if(rand_no > 50) {
					new_g[i * (gsize + 1) + gsize] = 1; // last column
					new_g[gsize * (gsize + 1) + i] = 1;	
				}
				else {
					new_g[i * (gsize + 1) + gsize] = 0; // last column
					new_g[gsize * (gsize + 1) + i] = 0;	
				}
			}
			gsize = gsize + 1;
			// reset the taboo list for the new graph
			FIFODelete(taboo_list);
			taboo_list = FIFOInitEdge(TABOOSIZE);
			best_count = BIGCOUNT;
		}
	}
//	FIFODeleteGraph(taboo_list);
//	FIFODeleteGraph(taboo_list_backtrace);
	return(0);
}

int recursiveSearch(int* g, int gsize, int level, int best_ever,
										 int cur_i, int cur_j) {
	if(level == 0) return false;
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
	for(i = 0; t < gsize; i ++) {
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
		return best_count;
	}
		
	getNeighbors(neighbor, g, gsize);
	// recursively check neighbor one by one
	for(k = 0; k < NEIGHBOR_SIZE; k ++) {
		nb_i = neighbor[k][0];
		nb_j = neighbor[k][1];
		count = recursiveSearch(g, gsize, level - 1, 
														best_ever, nb_i, nb_j)
		if(count != -1) {
			to_return = count;
			g[nb_i * gsize + nb_j] = 1 - g[nb_i * gsize + nb_j];
			break;
		}
	}		
	g[cur_i * gsize + cur_j] = 1 - g[cur_i * gsize + cur_j];
	return to_return;	
}















