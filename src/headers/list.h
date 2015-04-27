#ifndef _LIST_H_
#define _LIST_H_

typedef struct Clique_ {
	int* clique_node;
	int color;
	struct Clique_* next;
} Clique;

typedef struct List_ {
	unsigned int clique_size;
	Clique* front;
	Clique* back;
	long length;
} List;

/*
 * seach list to see if the cliques contains
 * edge(i, j). if a clique does not contain 
 * edge(i, j) and color is the same as the
 * clique's color, counter increments. The
 * counter is the number of new cliques that
 * will be generated if flip edge(i, j)
 */
int list_search_6(List *list,int *g, int g_size, int i, int j,  int stop);

/*
 * seach list to see if the cliques contains
 * edge(i, j). if a clique contains edge(i, j) 
 * and color is the same as the clique's color, 
 * the counter increments. The counter is the
 * number of cliques has to be removed from the
 * original clique count
 */
int list_search_7(List *list, int i, int j);

void list_add(List *list, Clique *clique);

void list_delete(List *list);
// inline
void clique_init_7(Clique *clique, int color, int i, int j, int k, 
				 	int l, int m, int n, int o);
void clique_init_6(Clique *clique, int color, int i, int j, int k, 
 				 	int l, int m, int n);


void list_init(List *list, int clique_size);
void print_list(List *list);

#endif
