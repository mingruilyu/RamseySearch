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
/*
 * example of very simple search for R(7, 7) counter 
 * examples starts with a small randomized graph and 
 * works its way up to successively larger graphs one 
 * at a time
 * uses a taboo list of size #TABOOSIZE# to hold and 
 * encoding of and edge (i, j) + clique_count
 */


int main(int argc, char *argv[])
{
	int *g, *new_g;
	int gsize, count, i, j, k, best_count, rand_no,
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
	CliqueCountCreateCache(g, gsize);
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
															best_i, best_j);
			if(count != -1) {
				printf("EVOLVNG!!!!!!!!!!\n");
				g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];
				PrintGraph(g, gsize);
				FIFOInsertEdgeCount(taboo_list, best_i, best_j, count);
				printf("best_count = %d, edge(%d, %d) \n", 
							 best_count, best_i, best_j, g[best_i * gsize + best_j]);
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













