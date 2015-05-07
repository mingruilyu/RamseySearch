#include "clique_count.h"
#include "stdio.h"
/*
 * returns the number of monochromatic 
 * cliques in the graph presented to it.
 * graph is stored in row-major order
 * only checks values above diagonal
 */

Clique6list cache_6;
Clique7list cache_7;

int CliqueCountFast(int* g, int gsize, int stop, long* clique_6) {
  int i, j, k, l, m, n, o;
  int count = 0;
	int color;
  int sgsize = 7; 
	*clique_6 = 0;
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
											*clique_6 = *clique_6 + 1;
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

int CliqueCount(int* g, int gsize) {
  int i, j, k, l, m, n, o;
  int count = 0;
	int color, sgsize = 7;  
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
												//	printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", i, j, k , l, m, n, o);
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

int CliqueCountUseCache(int *g, int gsize, int i, int j, int stop) {
	int clique_7to6_count = search_7( i, j);
	int clique_7to7_count = cache_7.length - clique_7to6_count;
	if(clique_7to7_count > stop) return -1;
	int clique_6to7_count = search_6(g, gsize, 
																				i, j, stop - clique_7to7_count);
//	printf("clique_6to7_count\t%d\n", clique_6to7_count);
	if(clique_6to7_count == -1) return -1;
//	int clique_7to6_count = list_search_7(cache_7, i, j);
	//printf("clique_7to6_count\t%d\n", clique_7to6_count);
	int clique_7 = clique_6to7_count + clique_7to7_count;
	if(clique_7 > stop)
		return -1;
	else return clique_7;
}

int CliqueCountCreateCache(int *g, int gsize) {
  int i, j, k, l, m, n, o;
  int count = 0, color;
	int sgsize = 7;
    int clique_6_counter=0;
    int Clique_7_counter=0;
  cache_6.length=0;
  cache_7.length = 0;

  for(i = 0; i < gsize - sgsize + 2; i ++) {
		for(j = i + 1; j < gsize - sgsize + 3; j ++) {
	  	for(k = j + 1; k < gsize - sgsize + 4; k ++) { 
				if((g[i * gsize + j] == g[i * gsize + k]) 
						&& (g[i * gsize + j] == g[j * gsize + k])) {
						color = g[i * gsize + j];
					for(l = k + 1; l < gsize - sgsize + 5; l ++) {
						if((color == g[i * gsize + l]) 
								&& (color == g[j * gsize + l]) 
								&& (color == g[k * gsize + l])) {
							for(m = l + 1; m < gsize - sgsize + 6; m ++) {
								if((color == g[i * gsize + m]) 
										&& (color == g[j * gsize + m]) 
										&& (color == g[k * gsize + m]) 
										&& (color == g[l * gsize + m])) {
			  					for(n = m + 1; n < gsize - sgsize + 7; n ++) {
										if((color == g[i * gsize + n]) 
												&& (color == g[j * gsize + n]) 
												&& (color == g[k * gsize + n]) 
												&& (color == g[l * gsize + n]) 
												&& (color == g[m * gsize + n])) {
											for(o = n + 1; o < gsize - sgsize + 8; o ++) {
												if((o < gsize - sgsize + 7) 
														&& (color == g[i * gsize + o]) 
														&& (color == g[j * gsize + o]) 
														&& (color == g[k * gsize + o]) 
														&& (color == g[l * gsize + o]) 
														&& (color == g[m * gsize + o]) 
														&& (color == g[n * gsize + o])) {
		      								count++;
													
                                                    //clique = clique_init_7(color, i, j, k, l, m, n, o);
                                                    
													//list_add(cache_7, clique);
                                                    add_clique_7(color, i, j, k, l, m, n, o);
                                                   
                                                    
                                                    
			  								} else {
													//if(i == 2 && j == 7 && k == 8 && l == 15 && m == 33 && n == 88) printf("find the clique\n");
													//clique = clique_init_6(color, i, j, k, l, m, n);
													//list_add(cache_6, clique);
                                                    //cache_6[Clique_6_counter++]=clique;
                                                    add_clique_6(color, i, j, k, l, m, n);
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
}

long CliqueCount6(int *g, int gsize, long stop) {
  int i, j, k, l, m, n, color;
	int sgsize = 6;
	long count = 0;
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
											count ++;
											/*if(count > stop)
												return -1;*/
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
	return count;
}
