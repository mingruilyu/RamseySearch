#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <strings.h>
#include <math.h>

/*
 * returns the number of monochromatic 
 * cliques in the graph presented to it.
 * graph is stored in row-major order
 * only checks values above diagonal
 */

int CliqueCount_7_cache(int *g, int gsize, List *cache_7) {
  int i, j, k, l, m, n, o;
  int count = 0, sgsize = 7, color;
 	Clique* clique;
  for(i = 0; i < gsize - sgsize + 1; i ++) {
		for(j = i + 1; j < gsize - sgsize + 2; j ++) {
	  	for(k = j + 1; k < gsize - sgsize + 3; k ++) { 
				if((g[i * gsize + j] == g[i * gsize + k]) 
						&& (g[i * gsize + j] == g[j * gsize + k])) {
						color = g[i * gsize + j]; 
					for(l = k + 1; l < gsize - sgsize + 4; l ++) {
						if((color == g[i * gsize + l]) 
								&& (color == g[j * gsize + l]) 
								&& (color == g[k * gsize + l])) {
							for(m = l + 1; m < gsize - sgsize + 5; m ++) {
								if((color == g[i * gsize + m]) 
										&& (color == g[j * gsize + m]) 
										&& (color == g[k * gsize + m]) 
										&& (color == g[l * gsize + m])) {
			  					for(n = m + 1; n < gsize - sgsize + 6; n ++) {
										if((color == g[i * gsize + n]) 
												&& (color == g[j * gsize + n]) 
												&& (color == g[k * gsize + n]) 
												&& (color == g[l * gsize + n]) 
												&& (color == g[m * gsize + n])) {
		          				for(o = n + 1; o < gsize - sgsize + 7;i o ++) {
												if((color == g[i * gsize + o]) 
														&&(color == g[j * gsize + o]) 
														&&(color == g[k * gsize + o]) 
														&&(color == g[l * gsize + o]) 
														&&(color == g[m * gsize + o]) 
														&&(color == g[n * gsize + o])) {
		      								count++;
													clique = (Clique *) malloc(sizeof(Clique));
													clique_init_7(clique, i, j, k, l, m, m, o);
													list_add(cache_7, clique);
			  								}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	cache_7->length = count;
	return(count);
}

int CliqueCount_6_cache(int g, int gsize, List* cache_6) {
  int i, j, k, l, m, n;
  int count = 0, sgsize = 6, color;
    
  for(i = 0; i < gsize - sgsize + 1; i ++) {
		for(j = i + 1; j < gsize - sgsize + 2; j ++) {
			color = g[i * gsize + j]; 
	  	for(k = j + 1; k < gsize - sgsize + 3; k ++) { 
				if((color == g[i * gsize + k]) 
						&& (color == g[j * gsize + k])) {
					for(l = k + 1; l < gsize - sgsize + 4; l ++) {
						if((color == g[i * gsize + l]) 
								&& (color == g[j * gsize + l]) 
								&& (color == g[k * gsize + l])) {
							for(m = l + 1; m < gsize - sgsize + 5; m ++) {
								if((color == g[i * gsize + m]) 
										&& (color == g[j * gsize + m]) 
										&& (color == g[k * gsize + m]) 
										&& (color == g[l * gsize + m])) {
			  					for(n = m + 1; n < gsize - sgsize + 6; n ++) {
										if((color == g[i * gsize + n]) 
												&& (color == g[j * gsize + n]) 
												&& (color == g[k * gsize + n]) 
												&& (color == g[l * gsize + n]) 
												&& (color == g[m * gsize + n])) {
		      							count++;
											clique = (Clique *) malloc(sizeof(Clique));
											clique_init_6(clique, i, j, k, l, m, m);
											list_add(cache_6, clique);
										}										
									}
								}
							}
						}
					}
				}
			}
		}
	}
	cache_6->length = count;
	return(count);
}

int CliqueCountFast(int g, int gsize, int stop) {
  int i, j, k, l, m, n, o;
  int count = 0;
	int color;
    
  for(i = 0; i < gsize - sgsize + 1; i ++) {
		for(j = i + 1; j < gsize - sgsize + 2; j ++) {
	  	for(k = j + 1; k < gsize - sgsize + 3; k ++) { 
				if((g[i * gsize + j] == g[i * gsize + k]) 
						&& (g[i * gsize + j] == g[j * gsize + k])) {
						color = g[i * gsize + j]; 
					for(l = k + 1; l < gsize - sgsize + 4; l ++) {
						if((color == g[i * gsize + l]) 
								&& (color == g[j * gsize + l]) 
								&& (color == g[k * gsize + l])) {
							for(m = l + 1; m < gsize - sgsize + 5; m ++) {
								if((color == g[i * gsize + m]) 
										&& (color == g[j * gsize + m]) 
										&& (color == g[k * gsize + m]) 
										&& (color == g[l * gsize + m])) {
			  					for(n = m + 1; n < gsize - sgsize + 6; n ++) {
										if((color == g[i * gsize + n]) 
												&& (color == g[j * gsize + n]) 
												&& (color == g[k * gsize + n]) 
												&& (color == g[l * gsize + n]) 
												&& (color == g[m * gsize + n])) {
		          				for(o = n + 1; o < gsize - sgsize + 7;o ++) {
												if((color == g[i * gsize + o]) 
														&&(color == g[j * gsize + o]) 
														&&(color == g[k * gsize + o]) 
														&&(color == g[l * gsize + o]) 
														&&(color == g[m * gsize + o]) 
														&&(color == g[n * gsize + o])) {
		      								count++;
													if(count >= stop) return (-1);
			  								}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return(count);
}

int CliqueCount(int g, int gsize) {
  int i, j, k, l, m, n, o;
  int count = 0;
	int color;
    
  for(i = 0; i < gsize - sgsize + 1; i ++) {
		for(j = i + 1; j < gsize - sgsize + 2; j ++) {
	  	for(k = j + 1; k < gsize - sgsize + 3; k ++) { 
				if((g[i * gsize + j] == g[i * gsize + k]) 
						&& (g[i * gsize + j] == g[j * gsize + k])) {
						color = g[i * gsize + j]; 
					for(l = k + 1; l < gsize - sgsize + 4; l ++) {
						if((color == g[i * gsize + l]) 
								&& (color == g[j * gsize + l]) 
								&& (color == g[k * gsize + l])) {
							for(m = l + 1; m < gsize - sgsize + 5; m ++) {
								if((color == g[i * gsize + m]) 
										&& (color == g[j * gsize + m]) 
										&& (color == g[k * gsize + m]) 
										&& (color == g[l * gsize + m])) {
			  					for(n = m + 1; n < gsize - sgsize + 6; n ++) {
										if((color == g[i * gsize + n]) 
												&& (color == g[j * gsize + n]) 
												&& (color == g[k * gsize + n]) 
												&& (color == g[l * gsize + n]) 
												&& (color == g[m * gsize + n])) {
		          				for(o = n + 1; o < gsize - sgsize + 7;o ++) {
												if((color == g[i * gsize + o]) 
														&&(color == g[j * gsize + o]) 
														&&(color == g[k * gsize + o]) 
														&&(color == g[l * gsize + o]) 
														&&(color == g[m * gsize + o]) 
														&&(color == g[n * gsize + o])) {
		      								count++;
			  								}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return(count);
}

int CliqueCountUseCache(int g, int gsize, int stop,
										 		List *cache_6, List *cache_7) {
		
}

int CliqueCountCreateCache(int g, int gsize, 
													 List *cache_6, List *cache_7) {
														 
}
