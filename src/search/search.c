#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "list.h"
#include "fifo.h"	/* for taboo list */
#include "time.h"
#include "clique_count.h"
#include "search.h"
#include "graph.h"
#include "recursion.h"
#define TABOOSIZE (500)
#define BIGCOUNT (1000)
#define RECURSION_THRESHOLD (50)
/*
 * example of very simple search for R(7, 7) counter 
 * examples starts with a small randomized graph and 
 * works its way up to successively larger graphs one 
 * at a time
 * uses a taboo list of size #TABOOSIZE# to hold and 
 * encoding of and edge (i, j) + clique_count
 */

int search(int *g, int gsize) {
	int count, i, j, best_count = BIGCOUNT, best_ever,
			best_i, best_j;
	int *new_g;
	void *taboo_list = FIFOInitEdge(TABOOSIZE);
	// best_ever is the goal we want to achieve,
	// it is lower than THRESHOLD
	CliqueCountCreateCache(g, gsize);
	// see how many clique 7 we have left. If there are still 
	// a lot, we will need to clean it up.
	if(cache_7.length == 0) {
		best_ever = RECURSION_THRESHOLD - 1;
		printf("Eureka! Counter-example found!\n");
		new_g = (int *) malloc((gsize + 1) * (gsize + 1) * sizeof(int));
		// copy the old graph into the new graph leaving 
		// the last row and last column alone
		CopyGraph(g, *gsize, new_g, *gsize + 1);
		// throw away the old graph and make new one 
		free(g);
		g = new_g;
		// randomly assigned value for last column
		for(i = 0; i < gsize + 1; i ++) {
			if(rand() % 100 > 50) {
				new_g[i * (gsize + 1) + gsize] = 1; // last column
				new_g[gsize * (gsize + 1) + i] = 1;	
			}
			else {
				new_g[i * (gsize + 1) + gsize] = 0; // last column
				new_g[gsize * (gsize + 1) + i] = 0;	
			}
		}
		gsize = gsize + 1;
	} else 
		best_ever = cache_7.length - 1;
	CliqueCountCreateCache(g, gsize);
	while(cache_7.length > RECURSION_THRESHOLD) {
		CliqueCountCreateCache(g, gsize);	
	// reset the taboo list for the new graph
		for(i = 0; i < gsize; i ++) {
			for(j = i + 1; j < gsize; j ++) {
      	g[i * gsize + j] = 1 - g[i * gsize + j];
				count = CliqueCountUseCache(g, gsize, i, j, best_count);
				if(count != -1 && !FIFOFindEdgeCount(taboo_list, i, j, count)) {
    	  	best_count = count;
          best_i = i;
          best_j = j;
				}
        g[i * gsize + j] = 1 - g[i * gsize + j];
			}
		} 
		printf("BACKTRACKING size: %d, best_6_count: %d, best_count: %d, best edge: (%d, %d), new color: %d\n",
					 gsize, cache_6.length, best_count, best_i, best_j, 
					 g[best_i * gsize + best_j]); 
		// keep the best flip we saw. 
		g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];
		CliqueCountCreateCache(g, gsize);
		FIFOInsertEdgeCount(taboo_list, best_i, best_j, best_count);
		best_count = BIGCOUNT;
		PrintGraph(g, gsize);
	}

	if(cache_7.length <= best_ever) {
		PrintGraphNew(g, gsize, new_graph_count);
		send_file();
		return (0);
	}
	for(i = 0; i < gsize; i ++) {
		for(j = i + 1; j < gsize; j ++) {
     	g[i * gsize + j] = 1 - g[i * gsize + j];
			count = CliqueCountUseCache(g, gsize, i, j, best_count);
			if(count != -1 && !FIFOFindEdgeCount(taboo_list, i, j, count)) {
    	 	best_count = count;
        best_i = i;
        best_j = j;
			}
      g[i * gsize + j] = 1 - g[i * gsize + j];
		}
	}
	recursiveSearch(g, gsize, RECURSION_DEPTH, best_ever, 
									best_i, best_j, taboo_list);
	return (0);
}

int *load_graph(int* gsize) {
	int *g;
	ReadGraph("../../file/client/old_graph", &g, gsize);
	return g;
}
