#ifndef _LIST_H_
#define _LIST_H_
#define CLIQUE_6_LIST_MAX_SIZE 1000000
#define CLIQUE_7_LIST_MAX_SIZE 1000



typedef struct Clique_6 {
	int clique_node[6];
	int color;
} Clique6;

typedef struct Clique_7 {
    int clique_node[7];
    int color;
} Clique7;

typedef struct Clique_6_list{
	Clique6 array[CLIQUE_6_LIST_MAX_SIZE];
	long length;
}Clique6list;

typedef struct Clique_7_list{
	Clique7 array[CLIQUE_7_LIST_MAX_SIZE];
	int length;
} Clique7list;

extern Clique6list cache_6;
extern Clique7list cache_7;




//typedef struct List_ {
//	unsigned int clique_size;
//	Clique* front;
//	Clique* back;
//	long length;
//} List;

#define MAKE_KEY(key_array, key, radix) \
	int it;												 \
	for(it = 6; it >= 0; it --) {   \
		key+= key_array[it] * radix; \
		radix *= 211; }		
/*
 * seach list to see if the cliques contains
 * edge(i, j). if a clique does not contain 
 * edge(i, j) and color is the same as the
 * clique's color, counter increments. The
 * counter is the number of new cliques that
 * will be generated if flip edge(i, j)
 */
//int list_search_6(List *list,int *g, int g_size,
									
int search_6(int *g, int g_size,
									int i, int j,  int stop);
int binary_search_first(Clique6 *array, int start, int end, int target);
/*
 * seach list to see if the cliques contains
 * edge(i, j). if a clique contains edge(i, j) 
 * and color is the same as the clique's color, 
 * the counter increments. The counter is the
 * number of cliques has to be removed from the
 * original clique count
 */
//int list_search_7(List *list, int i, int j);
int search_7( int i, int j);

//void list_add(List *list, Clique *clique);

//void list_delete(List *list);
// inline
void add_clique_7(int color, int i, int j,
                   int k, int l, int m,
                   int n, int o);
void add_clique_6(int color, int i, int j,
                   int k, int l, int m, int n);


//List* list_init(int clique_size);
//void print_list(List *list);

#endif
