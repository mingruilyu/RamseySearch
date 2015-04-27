#include <stdio.h>
#include <stdlib.h>
#include <list.h>
#include <stdbool.h>
/*
 * seach list to see if the cliques contains
 * edge(i, j). if a clique does not contain 
 * edge(i, j) and color is the same as the
 * clique's color, counter increments. The
 * counter is the number of new cliques that
 * will be generated if flip edge(i, j)
 */



int list_search_6(List *list, int *g, int g_size, int i, int j, int stop){
	int clique_size = list->clique_size;
	int counter = 0;

	if( clique_size!=6){
		fprintf(stdout,"Wrong clique_size");
		return -1;
	}

	Clique *current = list -> front;
	while (current != NULL && counter < stop) {  
		int *nodeArray = current -> clique_node;		
		bool inclique_i=false;
		bool inclique_j=false;
		int p;
		int in_index;

		for(p = 0; p < clique_size; p++){
			if(nodeArray[p] == i){
				inclique_i=true;
			}
			if(nodeArray[p]==j){
				inclique_j=true;
			}
		}


		int clique_color = current -> color;		
		if( inclique_j != inclique_i){
			if(inclique_j)
				in_index = j;
			else in_index = i;
			for( p = 0; p < clique_size; p++){
				if(g[nodeArray[p]*g_size+j]!=clique_color)
					break;
			}
			if(p==clique_size)
				counter++;
		}
	    current = current->next;

    }
    return counter;
}



/*
 * seach list to see if the cliques contains
 * edge(i, j). if a clique contains edge(i, j) 
 * and color is the same as the clique's color, 
 * the counter increments. The counter is the
 * number of cliques has to be removed from the
 * original clique count
 */
int list_search_7(List *list, int i, int j){
	int clique_size = list->clique_size;
	int counter = 0;

	if( clique_size!=7){
		fprintf(stdout,"Wrong clique_size");
		return -1;
	}
	Clique *current = list -> front;
	while (current != NULL) {  
		int *nodeArray = current -> clique_node;
		bool inclique_i;
		bool inclique_j;
		int p;

		for(p = 0; p < clique_size; p++){
			if(nodeArray[p] == i)
				inclique_i=true;
			if(nodeArray[p]==j)
				inclique_j=true;
		}		
		if( inclique_j && inclique_i)
			counter++;
	    current = current->next;
	}
	return counter;
}


void list_add(List *list, Clique *clique){

	Clique *tail = list->back;
	if(tail != NULL){
		tail->next = clique;
	}
	else{
		list->front = clique;		
	}
	list->back = clique;
	list->length = list->length +1;
	return;
	
}

void list_delete(List *list){
	Clique *toDelete = list -> front;
	Clique *next_clique= toDelete-> next;
	
	while(toDelete!=NULL){		
		free(toDelete);
		toDelete = next_clique;
		if(next_clique !=NULL)
			next_clique = next_clique->next;

	}	
	list_init(list, list->clique_size);
	return;
}

void print_list(List *list){
	Clique *toDelete = list -> front;
	fprintf(stdout," start print list \n ");
	int i;
	while(toDelete!=NULL){	
		int * array = toDelete-> clique_node;		
		for(i = 0; i<list->clique_size; i++){
			fprintf(stdout,"%d ", array[i]);
		}		
		toDelete = toDelete->next;	
	}	
	fprintf(stdout," end of print list \n ");
	return;
}


void list_init(List *list, int clique_size){
	//list =(List*) malloc(sizeof(List));
	list->front = NULL;
	list->back = NULL;
	list->clique_size = clique_size;
	list-> length = 0;
	return;
}

void clique_init_7(Clique *clique,int color, int i, int j, int k, 
				 	int l, int m, int n, int o){

	int *nodes; 
    nodes=(int*)malloc(7*sizeof(int*)); 
    nodes[0]=i;
    nodes[1]=j;
    nodes[2]=k;
    nodes[3]=l;
    nodes[4]=m;
    nodes[5]=n;
    nodes[6]=o;

	clique -> clique_node = nodes;
	clique -> color = color;
	clique -> next = NULL;
	return;
}
// inline
void clique_init_6(Clique *clique, int color, int i, int j, int k, 
				 	int l, int m, int n){
	 
	 int *nodes; 
     nodes=(int*)malloc(6*sizeof(int*)); 

     nodes[0]=i;
     nodes[1]=j;
     nodes[2]=k;
     nodes[3]=l;
     nodes[4]=m;
     nodes[5]=n;
	 clique -> clique_node = nodes;
	 clique -> color = color;
    clique -> next = NULL;
	 return;

}



int main( int argc, char *argv[])
{
	
	List *list;	
	list = (List*) malloc(sizeof(List));	
    list_init(list, 6);


    // fprintf(stdout,"3");
     Clique *clique1;     
     clique1 = (Clique*) malloc(sizeof(Clique));
     
     print_list(list);     
     clique_init_6(clique1, 0, 1,2,3,4,5,6);    

     list_add(list,clique1);
     	
     print_list(list);
     Clique *clique2;
     clique2 = (Clique*) malloc(sizeof(Clique));
     clique_init_6(clique2, 0, 2,3,4,5,6,7);
     list_add(list,clique2);
     print_list(list);
     //list_delete(list);
    // print_list(list);


   //  int counter = list_search_6(list, 1,2);
    // fprintf(stdout," counter = %d \n ",counter);
int *g =(int*)malloc(64*sizeof(int*)); 
int counter = list_search_6(list, g, 10, 10,  8,  100);
fprintf(stdout," counter = %d \n ",counter);
    // Clique *clique3;
    // clique3 = (Clique*) malloc(sizeof(Clique));
    // clique_init_6(clique3, 0, 3,4,5,6,7,8);
    // list_add(list,clique3); 
    return(0); 
}
