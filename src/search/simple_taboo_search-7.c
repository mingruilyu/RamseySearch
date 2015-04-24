#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "fifo.h"	/* for taboo list */
#include "time.h"
#include "clique_count.h"
#define MAXSIZE (541)
#define TABOOSIZE (500)
#define BIGCOUNT (9999999)
/***
 *** example of very simple search for R(7,7) counter examples
 ***
 *** starts with a small randomized graph and works its way up to successively
 *** larger graphs one at a time
 ***
 *** uses a taboo list of size #TABOOSIZE# to hold and encoding of and edge
 *** (i,j)+clique_count
 ***/

int
main(int argc,char *argv[])
{
	typedef enum {false, true} bool;
	int *g;
	int *new_g;
	int gsize;
	int count;
	int i;
	int j;
	int best_count;
	int best_i;
	int best_j;
	void *taboo_list;
	srand(time(NULL));
	int try_count = 0;
	void** taboo_matrix = (void**)malloc(500*500*sizeof(void*));
	int taboo_matrix_size = 0;
	bool degrade_flag = false;
	
	/*
	 * start with graph of size 8
	 */
	gsize = 8;
	g = (int *)malloc(gsize*gsize*sizeof(int));
	if(g == NULL) {
		exit(1);
	}
	new_g = g;
	taboo_list = FIFOInitEdge(TABOOSIZE);
	taboo_matrix[taboo_matrix_size] = taboo_list;
	taboo_matrix_size ++;
	/*
	 * make a fifo to use as the taboo list
	 */
	taboo_list = FIFOInitEdge(TABOOSIZE);
	taboo_matrix[taboo_matrix_size] = taboo_list;
	taboo_matrix_size ++;
	if(taboo_list == NULL) {
		exit(1);
	}

	/*
	 * start out with all zeros
	 */
	memset(g,0,gsize*gsize*sizeof(int));

	/*
	 * while we do not have a publishable result
	 */
	while(gsize < 206)
	{
		/*
		 * find out how we are doing
		 */
		count = CliqueCount(g,gsize);

		/*
		 * if we have a counter example
		 */
		if(!degrade_flag && count == 0)
		{
			printf("Eureka!  Counter-example found!\n");
			PrintGraph(g,gsize);
				/*
			 	 * make a new graph one size bigger
			 	 */
				new_g = (int *)malloc((gsize+1)*(gsize+1)*sizeof(int));
				if(new_g == NULL)
					exit(1);
				/*
			 	 * copy the old graph into the new graph leaving the
			 	 * last row and last column alone
			 	 */
				CopyGraph(g,gsize,new_g,gsize+1);
				/*
			 	 * throw away the old graph and make new one the
			 	 * graph
			 	 */
				free(g);
				g = new_g;

			/*
			 * zero out the last column and last row
			 */
			for(i=0; i < (gsize+1); i++)
			{
				int randNo = rand() % 100;
				if(randNo > 50) {
					new_g[i*(gsize+1) + gsize] = 1; // last column
					new_g[gsize*(gsize+1) + i] = 1; // last row
				} else {
					new_g[i*(gsize+1) + gsize] = 0; // last column
					new_g[gsize*(gsize+1) + i] = 0; // last row
				}
			}

			gsize = gsize + 1;
			try_count = 0;
			/*
			 * reset the taboo list for the new graph
			 */
			taboo_list = FIFOInitEdge(TABOOSIZE);
			taboo_matrix[taboo_matrix_size] = taboo_list;
			taboo_matrix_size ++;
			/*
			 * keep going
			 */
			continue;
		}
		if(degrade_flag) degrade_flag = false;
		/*
		 * otherwise, we need to consider flipping an edge
		 *
		 * let's speculative flip each edge, record the new count,
		 * and unflip the edge.  We'll then remember the best flip and
		 * keep it next time around
		 *
		 * only need to work with upper triangle of matrix =>
		 * notice the indices
		 */
		best_count = BIGCOUNT;
		j = gsize - 1;
		try_count ++;
		for(i = 0; i < gsize; i ++) {
			/*
			 * flip it
			 */
			g[i*gsize+j] = 1 - g[i*gsize+j];
			count = CliqueCount(g,gsize);
			/*
			 * is it better and the i,j,count not taboo?
			 */
			if(count < best_count) {
				if(!FIFOFindEdgeCount(taboo_list,i,j,count)) {
					best_count = count;
					best_i = i;
					best_j = j;
					if(count == 0)  {
						g[i*gsize+j] = 1 - g[i*gsize+j];
						break;
					}
				} else {
					printf("Already in the taboo list!\n");
				}
			}

			/*
			 * flip it back
			 */
			g[i*gsize+j] = 1 - g[i*gsize+j];
		}

		/*try count may also need a list*/
		if(best_count != 0 && try_count == gsize / 2) {
			try_count = 0;
				
			FIFODelete(taboo_matrix[taboo_matrix_size - 1]);
			taboo_matrix_size --;
			g = DegradeGraph(g, gsize);
			gsize --;
			taboo_list = taboo_matrix[taboo_matrix_size - 1];
			degrade_flag = true;
			printf("Not found in the outtest layer!\n\
					Move one layer inward\n\
					=====================\n");

			continue;
		}

		if(best_count == BIGCOUNT) {
			printf("no best edge found, terminating\n");
			exit(1);
		}
		
		/*
		 * keep the best flip we saw
		 * there is a bug here. If the best_i, best_j is
		 * not found, the last flip will be reversed!!!!
		 */
		g[best_i*gsize+best_j] = 1 - g[best_i*gsize+best_j];

		/*
		 * taboo this graph configuration so that we don't visit
		 * it again
		 */
		count = CliqueCount(g,gsize);
		FIFOInsertEdgeCount(taboo_list,best_i,best_j,count);
		printf("ce size: %d, best_count: %d, best edge: (%d,%d), new color: %d\n",
			gsize,
			best_count,
			best_i,
			best_j,
			g[best_i*gsize+best_j]);
		/*
		 * rinse and repeat
		 */
	}

	FIFODeleteGraph(taboo_list);


	return(0);

}
