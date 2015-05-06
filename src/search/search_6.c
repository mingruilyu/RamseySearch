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
#define TABOOSIZE (500)
#define BIGCOUNT (1000)
#define THRESHOLD (10)
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

void create_edge_stat(int stat[120][120], bool people[120]) {
	int i, j, k;
	int* node;
	for(i = 0; i < 120; i ++) {
		for(j = 0; j < 120; j ++)
			stat[i][j] = 0;
		people[i] = false;
	}

	for(i = 0; i < cache_7.length; i ++) {
		node = cache_7.array[i].clique_node;
		for(j = 0; j < 6; j ++) {
			for(k = j + 1; k < 7; k ++) {
				people[node[j]] = true;
				people[node[k]] = true;
				stat[node[j]][node[k]] ++;
			}
		}		
	}
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
		if(!backtrack_flag) {
			//best_count = BIGCOUNT;i
			try_count = 0;
			while(best_count != 0 && try_count != gsize / 2) {
				// scan the current graph and create cache
				CliqueCountCreateCache(g, gsize);
				// Only flip the outmost column
				j = gsize - 1;
				best_i = -2;
				//clique_6_min = 10000000000;
				for(i = 0; i < gsize - 1; i ++) {
					// flip it
					g[i	*	gsize	+	j] = 1 - g[i * gsize + j];
					count = CliqueCountUseCache(g, gsize, i, j, best_count);
					// count is either -1 or (not -1 and less than best_count)
					if(count != -1) {
						if(!FIFOFindEdgeCount(taboo_list, i, j, best_count)) {
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
					rand_list[memory_index][0] = best_i;
					rand_list[memory_index ++][1] = best_j;
			  	// taboo this graph configuration so that we 
					// don't visit it again
					FIFOInsertEdgeCount(taboo_list, best_i, best_j, best_count);
					printf("MOVING FORWARD size: %d, best_6_count: %d, best_count: %d, best edge: (%d,%d), new color: %d\n",
							 gsize, cache_6.length, best_count, best_i, best_j,
							 g[best_i * gsize + best_j]);
	
					try_count ++;
				} else {
					printf("JUMP out\n");
					break;
				}
			}
			// no matter we are going to backtrack or 
			// to go forward, we need to delete FIFO
			// anyway
			if(best_count != 0) {
				//if(best_count < 61) {
					printf("Not found in the outmost layer!\
							 start backtracing\n\
							 =====================================\n");	
					backtrack_flag = true;
					for(i = 0; i < cache_7.length; i ++) {
						node = cache_7.array[i].clique_node;
						printf("%d %d %d %d %d %d %d\n", node[0], node[1], node[2], node[3], node[4], node[5], node[6]);
					}
			/*	} else {
					printf("REGENERATING!!!!!!!!!!!!!1\n");
					regenerate_flag = true;
					backtrack_flag = false;
					j = gsize - 1;
          for(i = 0; i < j; i ++) {
						// randomly generate each edge of the last column
						// with equal probability
            rand_no = rand() % 100;
            if(rand_no > 50) g[i * gsize + gsize - 1] = 1;
            else g[i * gsize + gsize - 1] = 0; // last column
          }
          taboo_list = FIFOResetEdge(taboo_list);
          best_count = BIGCOUNT;
				}*/
			} else backtrack_flag = false;
		} else {
			// we need to backtrace. let's start from the 
			// outmost edges, and work all the way 
			// to the inner edges. At the begining, we set the 
			// best_count to 100. Thus, if a flip generate 
			// count greater than the best_count, we discard it.	
			// We'll then remember the best flip and keep it 
			// next time around
			printf("BACKTRACING!!!!!!!!!\n");
			//best_count = BIGCOUNT;
			try_count = 0;
			best_count = 100;	
			// directed search to kill most of clique 7
			while(best_count != 0) {
				//try_count ++;
				//CliqueCountCreateCache(g, gsize);
				/*for(i = 0; i < cache_7.length; i ++) {
					node = cache_7.array[i].clique_node;
					break_flag = false;
					for(j = 0; j < 6 & !break_flag; j ++) {
						for(k = j + 1; k < 7; k ++) {
							// try to flip it
							if(search_6(g, gsize, node[j], node[k], 0) == 0) {
								break_flag = true;
								g[node[j] * gsize + node[k]] = 1 - g[node[j] * gsize + node[k]];
								printf("KILL!\n");
								CliqueCountCreateCache(g, gsize);
								break;
							}
						}
					}
				}*/
				
				//CliqueCountCreateCache(g, gsize);
				// see how many clique 7 we have left. If there are still 
				// a lot, we will need to clean it up.
				if(cache_7.length == 0) {
					printf("CLEAN SHOT!!!!!!!!!!!!!!!\n");
					backtrack_flag = false;
					break;
				}
		//		} else {
					// do the second round of clean up by breaking the ties
					// since best_count is initialized to BIGCOUNT, we won't
					// have 
					//best_count = BIGCOUNT;
					best_i = -2;	
					CliqueCountCreateCache(g, gsize);
					break_flag = false;
					rand_count = 0;
				/*	for(j = gsize - 1; !break_flag && j >= 1; j --) {
						for(i = 0; i < j; i ++) {
							g[i * gsize + j] = 1 - g[i * gsize + j];
							count = CliqueCountUseCache(g, gsize, i, j, best_count);
							//count = CliqueCountFast(g, gsize, best_count, &clique_6);
					//		rand_count ++;
				//			printf("i = %d, j = %d, count %d\n", i, j, count);
							if(count != -1 && count < best_count && (!FIFOFindEdgeCount(rand_taboo_list, i, j, 100)) &&!FIFOFindEdgeCount(taboo_list, i, j, best_count)) {
								best_count = count;
								best_i = i;
								best_j = j;
								break_flag = true;
								g[i * gsize + j] = 1 - g[i * gsize + j];
								break;
							}
							g[i * gsize + j] = 1 - g[i * gsize + j];
						}
					}*/
					//printf("random count = %d\n", rand_count);
					for(i = 0; i < cache_7.length && !break_flag; i ++) {
						node = cache_7.array[i].clique_node;
						break_flag = false;
						for(j = 0; j < 6 && !break_flag; j ++) {
							for(k = j + 1; k < 7; k ++) {
								// try to flip it
					//	j = rand() % 6;
					//	do k = rand() % 7;
					//	while(k <= j);
								g[node[j] * gsize + node[k]] = 1 - g[node[j] * gsize + node[k]];
								count = CliqueCountUseCache(g, gsize, node[j], node[k], best_count);
								if(count != -1 && !FIFOFindEdgeCount(rand_taboo_list, node[j], node[k], 100) && !FIFOFindEdgeCount(taboo_list, node[j], node[k], best_count)) {
							//		if(best_count > count) {
								//	best_count = count;
								//	best_i = node[j];
								//	best_j = node[k];
										//g[node[j] * gsize + node[k]] = 1 - g[node[j] * gsize + node[k]];
							//		}
									if(count < best_count) {
										best_count = count;
										best_i = node[j];
										best_j = node[k];
										break_flag = true;
										g[node[j] * gsize + node[k]] = 1 - g[node[j] * gsize + node[k]];
										break;
									}
								} // else printf("Already in taboo list!\n");
								g[node[j] * gsize + node[k]] = 1 - g[node[j] * gsize + node[k]];
							}
						}
					}
					if(best_i == -2) {
						if(best_count < last_best) {
							printf("EVOLVNG!!!!!!!!!!\n");
							last_best = best_count;
							PrintGraph(g, gsize);
							FIFODelete(taboo_list);
							taboo_list = FIFOInitEdge(TABOOSIZE);
							FIFODelete(rand_taboo_list);
							rand_taboo_list = FIFOInitEdge(TABOOSIZE);
							create_edge_stat(stat, people);
						} else if(best_count >= last_best) {
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
						CliqueCountCreateCache(g, gsize);
					/*	get_hottest_edge(stat, people, &hot_i, &hot_j);
						g[hot_i * gsize + hot_j] = 1 - g[hot_i * gsize + hot_j];
						FIFOInsertEdgeCount(rand_taboo_list, hot_i, hot_j, 100);
						printf("MUTATED (%d, %d)\n", hot_i, hot_j);
						memory[memory_index][0] = hot_i;
						memory[memory_index ++][1] = hot_j;*/
					/*	get_hottest_edge(stat, people, &hot_i, &hot_j);
						g[hot_i * gsize + hot_j] = 1 - g[hot_i * gsize + hot_j];
						FIFOInsertEdgeCount(rand_taboo_list, hot_i, hot_j, 100);
						printf("MUTATED (%d, %d)\n", hot_i, hot_j);	
						// add to memory
						memory[memory_index][0] = hot_i;
						memory[memory_index ++][1] = hot_j;*/
						for(i = cache_7.length - 1; i >= 0 && !break_flag; i --) {
							node = cache_7.array[i].clique_node;
						//	printf("%d %d %d %d %d %d %d\n", node[0], node[1], node[2], node[3], node[4], node[5], node[6]);
							for(j = 5; j >= 0 && !break_flag; j --) {
								for(k = j + 1; k < 7; k ++) {
								//	printf("i = %d, j = %d, k = %d\n", i, j, k);
						//			rand_list[rand_list_index][0] = node[j];					
						//			rand_list[rand_list_index ++][1] = node[k];
							//		printf("(%d, %d, %d)\n", i, j, k);	
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
				/*			if(rand() % 100 > 95) {				
									j = rand() % 6;
									do k = rand() % 7;
									while(k <= j);
									g[node[j] * gsize + node[k]] = 1 - g[node[j] * gsize + node[k]];
									printf("MUTATED (%d, %d)\n", node[j], node[k]);	
									memory[memory_index][0] = node[j];
									memory[memory_index ++][1] = node[k];
							}*/
							//		break_flag = true;
									//break;
								}
							}
						}
						//for(i = 0; i < 1; i ++) {
					/*	rand_count = 0;
						do {
							rand_no = rand() % cache_7.length;
							rand_i = rand_list[rand_no][0];
							rand_j = rand_list[rand_no][1];
						//	printf("randn\n");
							if(!FIFOFindEdgeCount(rand_taboo_list, rand_i, rand_j, 100)) {
								g[rand_i * gsize + rand_j] = 1 - g[rand_i * gsize + rand_j];
								FIFOInsertEdgeCount(rand_taboo_list, rand_i, rand_j, 100);
								printf("MUTATED (%d, %d)\n", rand_i, rand_j);	
								// add to memory
								memory[memory_index][0] = rand_i;
								memory[memory_index ++][1] = rand_j;
								rand_count ++;
								//break;
							}
							
						} while(rand_count < 1);*/
				/*		for(i = 0; i < 20; i ++) {
							rand_i = rand() % (gsize - 1);
							rand_j = rand() % (gsize - 1 - rand_i) + rand_i;
							g[rand_i * gsize + rand_j] = 1 - g[rand_i * gsize + rand_j];
							printf("MUTATED (%d, %d)\n", rand_i, rand_j);	
							memory[memory_index][0] = rand_i;
							memory[memory_index ++][1] = rand_j;
						}*/
						best_count = BIGCOUNT;
					} else {
						printf("BACKTRACKING size: %d, best_6_count: %d, best_count: %d, best edge: (%d, %d), new color: %d\n",
								 gsize, cache_6.length, best_count, best_i, best_j, 
								 g[best_i * gsize + best_j]); 
						// keep the best flip we saw. 
						g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];
						FIFOInsertEdgeCount(taboo_list, best_i, best_j, best_count);
						memory[memory_index][0] = best_i;
						memory[memory_index ++][1] = best_j;
						//last_best = best_count;
					}
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
		}
	}
//	FIFODeleteGraph(taboo_list);
//	FIFODeleteGraph(taboo_list_backtrace);
	return(0);
}

















