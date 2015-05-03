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
#define BIGCOUNT (1000)

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
	int *g, *new_g, *backup_g;
	int gsize, count, i, j, rand_no, 
			best_count, best_i, best_j, try_count = 0,
			last_flip_i = -1, last_flip_j = -1, 
			last_flip_i_forward = -2,
			last_flip_j_forward = -2,
			mutation_col, t;
	int sa_index = 998;
	void *taboo_list;
	srand(time(NULL));
	bool backtrack_flag = false, break_flag = false, mutate_flag = false;
	long clique_6_min, clique_6_count;
	int rand_count;
	int last_best = 10000;
	int memory[500][2];
	int memory_index = 0;
	List *cache_6, *cache_7;
	best_count = BIGCOUNT;

	if(argc == 0) { 
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
		backup_g = (int *) malloc(gsize * gsize * sizeof(int));
		CopyGraph(g, gsize, backup_g, gsize);
		// we can choose to either increment or not from here.
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
	}
	// make a fifo to use as the taboo list
	taboo_list = FIFOInitEdge(TABOOSIZE);

	if(taboo_list == NULL) exit(1);
		// while we do not have a publishable result
	while(gsize < 206) {
		// keep flipping the outmost column until counter
		// example is found
		if(!backtrack_flag || mutate_flag) {
			mutate_flag = false;
			try_count = 0;
		//	best_count = BIGCOUNT;
			while(best_count != 0 && try_count != gsize / 2) {
				// scan the current graph and create cache
				// for 6 clique and 7 clique
				cache_6 = list_init(6);
				cache_7 = list_init(7);
				CliqueCountCreateCache(g, gsize, cache_6, cache_7);
				//best_count = CliqueCountFast(g, gsize, best_count);
				printf("cache_6 size: %d\n", cache_6->length);
				// Only flip the outmost column
				j = gsize - 1;
				best_i = -2;
				//clique_6_min = 10000000000;
				for(i = 0; i < gsize - 1; i ++) {
					if(i == last_flip_i_forward && j == last_flip_j_forward)
						continue;
					// flip it
					g[i	*	gsize	+	j] = 1 - g[i * gsize + j];
					count = CliqueCountUseCache(g, gsize, i, j, best_count, cache_6, cache_7);
					// count is either -1 or (not -1 and less than best_count)
					if(count != -1) {
						if(!FIFOFindEdgeCount(taboo_list, i, j, count)) {
							best_count = count;
							best_i = i;
							best_j = j;
							// we cannot do this any more. We need to
							// try the whole column to get the smallest
							// clique_6_count
							if(count == 0) {
								g[i * gsize + j] = 1 - g[i * gsize + j];
								break;
							}
						} else printf("FORWARDING: Already in the forward taboolist!\n");
					}
					// flip it back 
					g[i * gsize + j] = 1 - g[i * gsize + j];
				}
				if(best_i != -2) { 
					// this is necessary since if fliping any edge
					// does not make the best count better, it will
					// happen that the last flip will be undo
					// keep the best flip we saw
					g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];
					last_flip_i_forward = best_i;
					last_flip_j_forward = best_j;
					memory[memory_index][0] = best_i;
					memory[memory_index ++][1] = best_j;
			  	// taboo this graph configuration so that we 
					// don't visit it again
					FIFOInsertEdgeCount(taboo_list, best_i, best_j, best_count);
					printf("MOVING FORWARD size: %d, best_6_count: %d, best_count: %d, best edge: (%d,%d), new color: %d\n",
							 gsize, cache_6->length, best_count, best_i, best_j,
							 g[best_i * gsize + best_j]);
	
					try_count ++;
					list_delete(cache_6);
					list_delete(cache_7);
				} else {
					list_delete(cache_6);
					list_delete(cache_7);
					printf("JUMP out\n");
					break;
				}
			}
			// no matter we are going to backtrack or 
			// to go forward, we need to delete FIFO
			// anyway
			if(best_count != 0) {
				printf("Not found in the outmost layer!\
							 start backtracing\n\
							 =====================================\n");	
				backtrack_flag = true;
			} else backtrack_flag = false;
		} else {
			// we need to backtrace. let's start from the 
			// outmost edges, and work all the way 
			// to the inner edges. At the begining, we set the 
			// best_count to 100. Thus, if a flip generate 
			// count greater than the best_count, we discard it.	
			// We'll then remember the best flip and keep it 
			// next time around
			best_count = BIGCOUNT;	
			try_count = 0;
			// last_flip, actually we need to set best_count to zero and unflip the last edge 
			best_i = -3, best_j = -3;
			while(best_count != 0 && try_count != gsize * gsize / 2) {
				// it is possible here that if none of the edge is 
				// flipped. we may end up fliping the same edge back
				// and forth
				cache_6 = list_init(6);
				cache_7 = list_init(7);
				break_flag = false;
				CliqueCountCreateCache(g, gsize, cache_6, cache_7);	
				best_i = -2;
				for(j = gsize - 1; !break_flag && j >= 1; j --) {
					for(i = 0; i < j - 1; i ++) {
						if(i == last_flip_i && j == last_flip_j) continue;
			 			// flip it
						g[i * gsize + j] = 1 - g[i * gsize + j];
						count = CliqueCountUseCache(g, gsize, i, j, best_count, cache_6, cache_7);
			//			long clique_6_count;
						// count is either -1 or not -1 and less than best_count
							if(count >= 0 && count < best_count) {
								if(!FIFOFindEdgeCount(taboo_list, i, j, count)) {
									best_count = count;
									best_i = i;
									best_j = j;
								//if(count == 0)  {
									g[i * gsize + j] = 1 - g[i * gsize + j];
									break_flag = true; 
									break;
								} else printf("Already in the backtrace taboo list!\n");
							} else if(count == best_count) {
							//	printf("i = %d, j = %d, count = %d, best_j = %d\n", i, j, count, best_j);
								if(best_j <= j) {
									if(!FIFOFindEdgeCount(taboo_list, i, j, best_count)) {
										printf("SET i = %d, j = %d\n", i, j);
										best_i = i;
										best_j = j;
									} else printf("Already in the backtrace taboo list!\n");
								}
							}
						// flip it back
						g[i * gsize + j] = 1 - g[i * gsize + j]; 
					}
				}
				if(best_i == -2) {
					if(best_count > last_best) {
						for(t = 0; t < memory_index; t ++) {
							i = memory[t][0];
							j = memory[t][1];
            	printf("RESTORING (%d, %d)\n", i, j);
           		g[i * gsize + j] = 1 - g[i * gsize + j];
						}
					} else {
						printf("EVOLVE!!!!!!!!!!!!!!!!!!!!!!!!\n");
						last_best = best_count;
						if(best_count < last_best)
							taboo_list = FIFOResetEdge(taboo_list);
					}
					memory_index = 0;
        	printf("MUTATE!!!!!!!!!!!!\n");
          for(rand_count = 0; rand_count < 1; rand_count ++) {
          	i = rand() % (gsize - 1);
            j = rand() % (gsize - 1 - i) + i + 1;
            printf("MUTATING (%d, %d)\n", i, j);
						memory[memory_index][0] = i;
						memory[memory_index ++][1] = j;
           	g[i * gsize + j] = 1 - g[i * gsize + j];
					}         
					best_count = BIGCOUNT;
					mutate_flag = true;
					taboo_list = FIFOResetEdge(taboo_list);
					backtrack_flag = false;
					break;
				}

				// Since we set best_i and best_j to negative, we
				// may need to check here. Another possibility is 
				// that if no count better than the best count is 
				// found, we won't be able to change any of the edge
				// Here comes an dead lock. Thus, we need to allow 
				// degenerate. 
				printf("BACKTRACKING size: %d, best_6_count: %d, best_count: %d, best edge: (%d, %d), new color: %d\n",
							 gsize, cache_6->length, best_count, best_i, best_j, 
							 g[best_i * gsize + best_j]); 
				// keep the best flip we saw there is a bug here. 
				g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];
				last_flip_i = best_i;
				last_flip_j = best_j;
				memory[memory_index][0] = best_i;
				memory[memory_index ++][1] = best_j;
		  	// taboo this graph configuration so that we 
				// don't visit it again
				printf("INSERTING best_count: %d, best edge: (%d, %d)\n", best_count, best_i, best_j);
				FIFOInsertEdgeCount(taboo_list, best_i, best_j, best_count);
				list_delete(cache_6);
				list_delete(cache_7);
				try_count ++;
				if(((FIFO_t *)taboo_list)->fcount > 100) {
					// if size taboo list is big, this means
					// the last column is not good, we need to
					// restore the graph and try a new one
					g = (int *) DegradeGraph(g, gsize);
					gsize --;
					CopyGraph(backup_g, gsize, g, gsize);
					mutate_flag = false;
					break;
				}
			}
			/*if(best_count != 0) {
				printf("no best edge found, terminating\n");
				exit(1);
			} else*/
			backtrack_flag = false;
		}

		// if we have a counter example.
		if(!backtrack_flag && !mutate_flag)	{
			printf("Eureka! Counter-example found!\n");
			PrintGraph(g, gsize);
			backup_g = (int *) malloc(gsize * gsize * sizeof(int));
			CopyGraph(g, gsize, backup_g, gsize);
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

















