#ifndef _LIST_H_
#define _LIST_H_

struct Clique {
	int* clique_edge;
	int color;
	Clique* next;
}

struct List {
	unsigned int clique_size;
	Clique* front;
	Clique* back;
	long length;
}

/*
 * seach list to see if the cliques contains
 * edge(i, j). if a clique does not contain 
 * edge(i, j) and color is the same as the
 * clique's color, counter increments. The
 * counter is the number of new cliques that
 * will be generated if flip edge(i, j)
 */
int list_search_6(List *list, int i, int j, int color);

/*
 * seach list to see if the cliques contains
 * edge(i, j). if a clique contains edge(i, j) 
 * and color is the same as the clique's color, 
 * the counter increments. The counter is the
 * number of cliques has to be removed from the
 * original clique count
 */
int list_search_7(List *list, int i, int j, int color);

void list_add(List *list, Clique *clique);

void list_delet(List *list);
// inline
void clique_init_7(Clique *clique, int i, int j, int k, 
				 	int l, int m, int n, int o);
// inline
void clique_init_6(Clique *clique, int i, int j, int k, 
				 	int l, int m, int n);
//inline
void clique_check(Clique *clique, int i, int j);
