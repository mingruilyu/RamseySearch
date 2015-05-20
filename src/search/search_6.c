#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
//#include <stdbool.h>
#include <string.h>
#include "list.h"
#include "fifo.h"	/* for taboo list */
#include "time.h"
#include "clique_count.h"
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
typedef enum {false, true} bool;
int rand_taboo_check(int i, int j, int list[100][2], int size) {
	int index;
	for(index = 0; index < size; index ++) {
		if(i == list[index][0] && j == list[index][1])
			return index;
	}
	return -1;
}

int create_edge_stat(int stat[120][120], bool people[120], int gsize) {
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

void get_hottest_edge(int stat[120][120], bool people[120], int *a, int *b) {
	int i, j, hot_i, hot_j, count = -1, ptr = 0;
	// here may be a bug
	for(i = 0; i < 120; i ++) {
		if(people[i]) {
			for(j = i + 1; j < 120; j ++) {
				if(stat[i][j] > count) {
					hot_i = i;
					hot_j = j;
					count = stat[i][j];
				}
			}
		}
	}
	stat[hot_i][hot_j] = 0;	
	*a = hot_i;
	*b = hot_j;
}

int main(int argc, char *argv[])
{
	int *g, *new_g;
	int gsize, count, i, j, k, rand_no, 
			best_count, best_i, best_j,
			t, try_count = 0, rand_count = 0;
	void *taboo_list, *rand_taboo_list;
	srand(time(NULL));
	bool backtrack_flag = false, break_flag = false, regenerate_flag = false;
	int rand_i, rand_j;
	int last_best = 10000;
	int rand_list[5000][2];
	int rand_list_index = 0;
	int memory[500][2];
	int memory_index = 0;
	int* node;
	long clique_6;
	int stat[120][120];
	bool people[120];
	int hot_i, hot_j;
	int edge_count;
	best_count = BIGCOUNT;
	int best_ever = BIGCOUNT + 1;
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
		last_best = BIGCOUNT;
		best_ever = cache_7.length;
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
	rand_taboo_list = FIFOInitEdge(TABOOSIZE);

	if(taboo_list == NULL) exit(1);
		// while we do not have a publishable result
	while(gsize < 206) {
		// keep flipping the outmost column until counter
		// example is found
		try_count = 0;
		//best_count = BIGCOUNT;	
			// directed search to kill most of clique 7
		CliqueCountCreateCache(g, gsize);
		while(best_count != 0) {
				
			CliqueCountCreateCache(g, gsize);
				// see how many clique 7 we have left. If there are still 
				// a lot, we will need to clean it up.
			if(cache_7.length < 10) {
				printf("CLEAN SHOT!!!!!!!!!!!!!!!\n");
				backtrack_flag = false;
				break;
			}
			// do the second round of clean up by breaking the ties
			// since best_count is initialized to BIGCOUNT, we won't
			// have 
			best_i = -2;	
			edge_count = create_edge_stat(stat, people, gsize);
			break_flag = false;
			for(i = gsize - 2; i >= 0 && !break_flag; i --) {
        if(people[i]) {
          for(j = i + 1; j < gsize && !break_flag; j ++) {
            if(stat[i][j] != 0) {
              //printf("edge count %d i = %d j = %d \n", edge_count, i, j);
              g[i * gsize + j] = 1 - g[i * gsize + j];
							count = CliqueCountUseCache(g, gsize, i, j, best_count);
							if(count != -1 && !FIFOFindEdgeCount(rand_taboo_list, i, j, 100) && !FIFOFindEdgeCount(taboo_list, i, j, count)) {
			//				if(count != -1 && !FIFOFindEdgeCount(taboo_list, i, j, count)) {
				//			if(count != -1 && !FIFOFindEdgeCount(taboo_list, i, j, 100)) {
                if(count < best_count) {
                  best_count = count;
                  best_i = i;
                  best_j = j;
        //          break_flag = true;
         //         g[i * gsize + j] = 1 - g[i * gsize + j];
          //        break;
                }
              }
              g[i * gsize + j] = 1 - g[i * gsize + j];
            }
          }
        }
      }
	/*		for(i = 0; i < gsize; i ++) {
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
			} */

							
			if(best_i == -2) {
			//	printf("nothing found! terminated!");
			//	exit(0);
				//CliqueCountCreateCache(g, gsize);
			//	if(best_count < last_best || (rand() % 100 > (70 + (best_count - best_ever)))) {
		//		if(best_count < last_best || (best_count - best_ever) < 3) {
				if(best_count < last_best) {
					if(best_count < best_ever) {
						printf("EVOLVNG!!!!!!!!!!\n");
						PrintGraph(g, gsize);
						best_ever = best_count;
					}
					printf("best_count = %d, last_best = %d, best_ever = %d\n", best_count, last_best, best_ever);
					last_best = best_count;
					FIFODelete(taboo_list);
					taboo_list = FIFOInitEdge(TABOOSIZE);
					FIFODelete(rand_taboo_list);
					rand_taboo_list = FIFOInitEdge(TABOOSIZE);
					//create_edge_stat(stat, people);
				} else if(best_count > last_best) {
					printf("RESTORING!!!!!!!!!!\n");
					for(i = 0; i < memory_index; i ++) {
						g[memory[i][0] * gsize + memory[i][1]] = 1 - g[memory[i][0] * gsize + memory[i][1]];
						printf("RESTORED (%d, %d)\n", memory[i][0], memory[i][1]);	
					}
				}
					// random flip 2 edges in the cache_7
						// add all edges into rand_list
				printf("MUTATING!!!!!!!!!!\n");
				rand_list_index = 0;
				memory_index = 0;
				// establish the random list
				break_flag = false;
				rand_count = 0;
				while(rand_count < 1) {
					i = rand() % (gsize - 1);
					j = rand() % (gsize - 1 - i) + i + 1;
					if(!FIFOFindEdgeCount(rand_taboo_list, i, j, 100)) { 
						break_flag = true;
						g[i * gsize + j] = 1 - g[i * gsize + j];
						FIFOInsertEdgeCount(rand_taboo_list, i, j, 100);
						printf("MUTATED (%d, %d)\n", i, j);	
						// add to memory
						memory[memory_index][0] = i;
						memory[memory_index ++][1] = j;
						rand_count ++;
					}
				}
			/*	while(!break_flag) {
					i = rand() % cache_7.length;
					node = cache_7.array[i].clique_node;
					for(j = 5; j >= 0 && !break_flag; j --) {
						for(k = j + 1; k < 7; k ++) {
							if(!FIFOFindEdgeCount(rand_taboo_list, node[j], node[k], 100)) {
								break_flag = true;
								g[node[j] * gsize + node[k]] = 1 - g[node[j] * gsize + node[k]];
								FIFOInsertEdgeCount(rand_taboo_list, node[j], node[k], 100);
								printf("MUTATED (%d, %d)\n", node[j], node[k]);	
									// add to memory
								memory[memory_index][0] = node[j];
								memory[memory_index ++][1] = node[k];
								break;
							}
						}
					}
				}*/
				best_count = BIGCOUNT;
			} else {
				printf("BACKTRACKING size: %d, best_6_count: %d, best_count: %d, best edge: (%d, %d), new color: %d\n",
							 gsize, cache_6.length, best_count, best_i, best_j, 
							 g[best_i * gsize + best_j]); 
						// keep the best flip we saw. 
				g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];
				FIFOInsertEdgeCount(taboo_list, best_i, best_j, best_count);
				//FIFOInsertEdgeCount(taboo_list, best_i, best_j, 100);
				best_count = BIGCOUNT;
			//	PrintGraph(g, gsize);
				memory[memory_index][0] = best_i;
				memory[memory_index ++][1] = best_j;
			}
		}
		//}

		// if we have a counter example.
		if(!backtrack_flag && !regenerate_flag)	{
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
			last_best  = BIGCOUNT;
			best_ever  = BIGCOUNT;
		}
	}
//	FIFODeleteGraph(taboo_list);
//	FIFODeleteGraph(taboo_list_backtrace);
	return(0);
}

bool recursiveSearch(int* g, int gsize, int level, int best_count,
										 int i, int j, int* new_i, int* new_j) {
	if(level == 0) return false;
	// flip it
	int neighbor[NEIGHBOR_SIZE][2];
	int k, nb_i, nb_j;
	bool to_return = false;
	CliqueCountCreateCache(g, gsize);
	getNeighbors(neighbor, g, gsize);
	for(k = 0; k < NEIGHBOR_SIZE; k ++) {
		nb_i = neighbor[k][0];
		nb_j = neighbor[k][1];
		g[nb_i * gsize + nb_j] = 1 - g[nb_i * gsize + nb_j];
		create_edge_stat(stat, people, gsize) {
		if(count != -1) {
			to_return = true;
			*new_i = nb_i;
			*new_j = nb_j;
			break;
		}
		if(recursiveSearch(g, gsize, level - 1, best_count, 
										nb_i, nb_j, new_i, new_j)) {
			to_return = true;
			*new_i = nb_i;
			*new_j = nb_j;
			break;
		}
		g[nb_i * gsize + nb_j] = 1 - g[nb_i * gsize + nb_j];
	}		
	return to_return;	
}















