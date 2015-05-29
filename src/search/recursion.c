#include "recursion.h"
#include "list.h"
#include "fifo.h"
#include "clique_count.h"
#include "client_transfer.h"
#include "graph.h"
#include <stdio.h>
#define RECURSION_RETURN_FAIL    -1
#define RECURSION_RETURN_SUCCESS -2
#define RECURSION_RETURN_TERMINATION -3
extern bool recv_flag;
int edges[200][200];
bool vertex[200];
int findNeighbours(int* g, int gsize, int cur_i, int cur_j, int neighbours[][NEIGHBOR_PARAM], void *taboo_list){
	int i,j,k,count, best_count, counter;
	counter = 0;
	for(i = 0; i < NEIGHBOR_SIZE; i ++)
		neighbours[i][NEIGHBOR_PARAM-1] =100000;
	i = cur_i;
	for(j = cur_j + 1; j < gsize; j ++) {
		best_count = neighbours[NEIGHBOR_SIZE-1][NEIGHBOR_PARAM-1];
		g[i * gsize + j] = 1 - g[i * gsize + j];
		count = CliqueCountUseCache(g, gsize, i, j, best_count);
		if(count != -1 && !FIFOFindEdgeCount(taboo_list, i, j, count)) {
			for(k=0; k < NEIGHBOR_SIZE; k++){
				if(count < neighbours[k][NEIGHBOR_PARAM-1]){
					counter++;
					shift(neighbours,k);
					neighbours[k][2]=count;
					neighbours[k][0]=i;
					neighbours[k][1]=j;
					break;					
				}
			}
		}
		g[i * gsize + j] = 1 - g[i * gsize + j];
	}
	for(i = cur_i+1; i < gsize; i ++) {
		for(j = i + 1; j < gsize; j ++) {
			best_count = neighbours[NEIGHBOR_SIZE-1][NEIGHBOR_PARAM-1];
			g[i * gsize + j] = 1 - g[i * gsize + j];
			count = CliqueCountUseCache(g, gsize, i, j, best_count);
			if(count != -1 && !FIFOFindEdgeCount(taboo_list, i, j, count)) {
				for(k=0; k < NEIGHBOR_SIZE; k++){
					if(count < neighbours[k][NEIGHBOR_PARAM-1]){
						counter++;
						shift(neighbours,k);
						neighbours[k][2]=count;
						neighbours[k][0]=i;
						neighbours[k][1]=j;
						break;					
					}
				}
			}
			g[i * gsize + j] = 1 - g[i * gsize + j];
		}
	}
	if(counter < NEIGHBOR_SIZE)
		return counter;
	return NEIGHBOR_SIZE;
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
	//printf("number of edges: %d \n", count);
	return count;
}


int recursiveSearch(int* g, int gsize, int level, int best_ever,
                    int cur_i, int cur_j, void *taboo_list) {
	int neighbor[NEIGHBOR_SIZE][3];
  int k, nb_i, nb_j, i, j, best_i, best_j, best_count, count;
  int to_return = RECURSION_RETURN_FAIL;
	if(recv_flag == true) return RECURSION_RETURN_TERMINATION; 
    // flip current edge
 	g[cur_i * gsize + cur_j] = 1 - g[cur_i * gsize + cur_j];
  CliqueCountCreateCache(g, gsize);
	if(cache_7.length > best_ever + 2) {
   	g[cur_i * gsize + cur_j] = 1 - g[cur_i * gsize + cur_j];
		return to_return;
	}
	printf(" RECURSION gsize %d in level %d edge (%d, %d) best_ever %d current_count %d \n", 
				 gsize, level, cur_i, cur_j, best_ever, cache_7.length);
    //check whether this flip itself will reduce clique 7 count
  if(cache_7.length <= best_ever) {
		to_return = RECURSION_RETURN_SUCCESS;
		PrintGraphNew(g, gsize);
		FIFOInsertEdgeCount(taboo_list, nb_i, nb_j, count);
		if(send_file(ip_addr) != 0)
			printf("Failed to send graph!\n");
	}
  // check upon this flip, is there any chance we may reduce
  // clique 7 count by flip another edge
/* create_edge_stat(gsize);
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
  if(best_count <= best_ever) {
        // we need to flip this edge, and leave the caller
        // of this function to flip the i, j
  	g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];
   // g[cur_i * gsize + cur_j] = 1 - g[cur_i * gsize + cur_j];
    FIFOInsertEdgeCount(taboo_list, best_i, best_j, best_count);
    printf("best_count = %d, edge(%d, %d) \n",
           best_count, best_i, best_j);
		to_return = RECURSION_RETURN_SUCCESS;
		PrintGraphNew(g, gsize, new_graph_count);
		FIFOInsertEdgeCount(taboo_list, nb_i, nb_j, count);
		if(send_file(ip_addr) != 0)
			printf("Failed to send graph!\n");
    //return best_count;
  }*/
  
	if(level == 0) {
 		g[cur_i * gsize + cur_j] = 1 - g[cur_i * gsize + cur_j];
		return RECURSION_RETURN_FAIL;
	}
  int neighbourCounter = findNeighbours(g, gsize, cur_i, cur_j, neighbor,taboo_list);
    // recursively check neighbor one by one
  for(k = 0; k < neighbourCounter; k ++) {
		nb_i = neighbor[k][0];
		nb_j = neighbor[k][1];
    count = recursiveSearch(g, gsize, level - 1,
                            best_ever, nb_i, nb_j,
														taboo_list);
    if(count == RECURSION_RETURN_TERMINATION) {
			to_return = RECURSION_RETURN_TERMINATION;
			break;
		}/* else if(count >= 0) {
			to_return = RECURSION_RETURN_SUCCESS;
			FIFOInsertEdgeCount(taboo_list, nb_i, nb_j, count);
    	g[nb_i * gsize + nb_j] = 1 - g[nb_i * gsize + nb_j];
			PrintGraphNew(g, gsize, new_graph_count);
			if(send_file(ip_addr) != 0)
				printf("Failed to send graph!\n");
		}*/
	}		
  g[cur_i * gsize + cur_j] = 1 - g[cur_i * gsize + cur_j];
  return to_return;	
}




