#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "list.h"
#include "fifo.h"	/* for taboo list */
#include "time.h"
#include "clique_count.h"
#define MAXSIZE (541)
#define TABOOSIZE (500)
#define BIGCOUNT (200)

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
	typedef enum {false, true} bool;
	int *g, *new_g;
	int gsize, count, i, j, rand_no, 
			best_count, best_i, best_j;
	void *taboo_list_forward, *taboo_list_backtrace;
	srand(time(NULL));
	int try_count = 0;
	bool backtrack_flag = false;
	List *cache_6, *cache_7;
	
	// start with graph of size 8
	gsize = 8;
	best_count = BIGCOUNT;
	g = (int *) malloc(gsize * gsize * sizeof(int));
	if(g == NULL) exit(1);
	new_g = g;

	// make a fifo to use as the taboo list
	taboo_list_forward = FIFOInitEdge(TABOOSIZE);
	taboo_list_backtrace = FIFOInitEdge(TABOOSIZE);

	if(taboo_list_forward == NULL 
		 || taboo_list_backtrace == NULL) exit(1);

	// start out with a random matrix
	for(i = 0; i < gsize; i ++) {
		for(j = i + 1; j < gsize; j ++) {
				rand_no = rand() % 100;
				if(rand_no > 50) g[i * gsize + j] = 1; 
				else new_g[i * gsize + j] = 0; 
		}
	}

	// while we do not have a publishable result
	while(gsize < 206)
	{
		// keep flipping the outmost column until counter
		// example is found
		if(!backtrack_flag) {
			try_count = 0;
			best_count = BIGCOUNT;
			while(best_count != 0 && try_count != gsize / 2) {
				// scan the current graph and create cache
				// for 6 clique and 7 clique
				cache_6 = list_init(6);
				cache_7 = list_init(7);
				CliqueCountCreateCache(g, gsize, cache_6, cache_7);	
				// Only flip the outmost column
				j = gsize - 1;
				for(i = 0; i < gsize - 1; i ++) {
					// flip it
					g[i	*	gsize	+	j] = 1 - g[i * gsize + j];
					count = CliqueCountUseCache(g, gsize, i, j, best_count, cache_6, cache_7);
					// count is either -1 or (not -1 and less than best_count)
					if(count == -1) {
						g[i * gsize + j] = 1 - g[i * gsize + j];
						continue;
					} else if(!FIFOFindEdgeCount(taboo_list_forward, i, j, count)) {
						best_count = count;
						best_i = i;
						best_j = j;
						if(count == 0)  {
							g[i * gsize + j] = 1 - g[i * gsize + j];
							break;
						}
					} else printf("Already in the forward taboo list!\n");
					// flip it back 
					g[i * gsize + j] = 1 - g[i * gsize + j];
				}
				// keep the best flip we saw
				g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];

			  // taboo this graph configuration so that we 
				// don't visit it again
				FIFOInsertEdgeCount(taboo_list_forward, best_i, best_j, best_count);
				printf("MOVING FORWARD size: %d, best_count: %d, \
							 best edge: (%d,%d), new color: %d\n",
							 gsize, best_count, best_i, best_j,
							 g[best_i * gsize + best_j]);
	
				try_count ++;
				list_delete(cache_6);
				list_delete(cache_7);
			}
			if(best_count != 0) {
				FIFODelete(taboo_list_forward);
				g = (int *) DegradeGraph(g, gsize);
				gsize --;
				printf("Not found in the outmost layer!\
							 start backtracing\n\
							 =====================================\n");	
				backtrack_flag = true;
			} else {
				backtrack_flag = false;
			}
		} else {
			// we need to backtrace. let's start from the 
			// outmost edges, new count, and work all the way 
			// to the inner edges. At the begining, we set the 
			// best_count to 100. Thus, if a flip generate 
			// count greater than the best_count, we discard it.	
			// We'll then remember the best flip and keep it 
			// next time around
			
			try_count = 0;
			best_count = 100;
			j = gsize - 1;
			while(best_count != 0 && try_count != gsize * gsize / 2) {
				cache_6 = list_init(6);
				cache_7 = list_init(7);
				CliqueCountCreateCache(g, gsize, cache_6, cache_7);	
				for(j = gsize - 1; j >= 1; j --) {
					for(i = 0; i < j - 1; i ++) {
			 			// flip it
						g[i * gsize + j] = 1 - g[i * gsize + j];
						count = CliqueCountUseCache(g, gsize, i, j, best_count, cache_6, cache_7);
						// count is either -1 or not -1 and less than best_count
						if(count == -1) continue;
						else if(!FIFOFindEdgeCount(taboo_list_backtrace, i, j, count)) {
							best_count = count;
							best_i = i;
							best_j = j;
							if(count == 0)  {
								g[i * gsize + j] = 1 - g[i * gsize + j];
								break;
							}
						} else printf("Already in the backtrace taboo list!\n");
						// flip it back
						g[i * gsize + j] = 1 - g[i * gsize + j]; 
					}
				}
				printf("BACKTRACKING size: %d, best_count: %d,\
							 best edge: (%d, %d), new color: %d\n",
							 gsize, best_count, best_i, best_j, 
							 g[best_i * gsize + best_j]); 
				// keep the best flip we saw there is a bug here. 
				g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];

		  	// taboo this graph configuration so that we 
				// don't visit it again
				FIFOInsertEdgeCount(taboo_list_backtrace, best_i, best_j, count);
				list_delete(cache_6);
				list_delete(cache_7);
				try_count ++;
			}
			if(best_count != 0) {
				printf("no best edge found, terminating\n");
				exit(1);
			} else backtrack_flag = false;
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
				if(rand_no > 50) new_g[i * (gsize + 1) + gsize] = 1; // last column
				else new_g[i * (gsize + 1) + gsize] = 0; // last column
			}
			gsize = gsize + 1;
			// reset the taboo list for the new graph
			taboo_list_forward = FIFOInitEdge(TABOOSIZE);
		}
	}
	FIFODeleteGraph(taboo_list_forward);
	FIFODeleteGraph(taboo_list_backtrace);
	return(0);
}
