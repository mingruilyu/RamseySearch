#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <stdbool.h>
#include "jrb.h"


/*
 * seach list to see if the cliques contains
 * edge(i, j). if a clique does not contain 
 * edge(i, j) and color is the same as the
 * clique's color, counter increments. The
 * counter is the number of new cliques that
 * will be generated if flip edge(i, j)
 */


void add_clique_7(int color, int i, int j,
											int k, int l, int m, 
											int n, int o){

	int *nodes; 
	int index = cache_7.length;
	//Clique* clique = (Clique *) malloc(sizeof(Clique));
    cache_7.array[index].clique_node[0]=i;
    cache_7.array[index].clique_node[1]=j;
    cache_7.array[index].clique_node[2]=k;
    cache_7.array[index].clique_node[3]=l;
    cache_7.array[index].clique_node[4]=m;
    cache_7.array[index].clique_node[5]=n;
    cache_7.array[index].clique_node[6]=o;
    cache_7.array[index].color=color;
    cache_7.length++;
	return;
}
// inline
void add_clique_6( int color, int i, int j,
											int k, int l, int m, int n) {
    
    int *nodes;
    int index = cache_6.length;
    //Clique* clique = (Clique *) malloc(sizeof(Clique));
    cache_6.array[index].clique_node[0]=i;
    cache_6.array[index].clique_node[1]=j;
    cache_6.array[index].clique_node[2]=k;
    cache_6.array[index].clique_node[3]=l;
    cache_6.array[index].clique_node[4]=m;
    cache_6.array[index].clique_node[5]=n;
    cache_6.array[index].color=color;
    cache_6.length++;
    return;

}

// keys are big double that is 250 radix.  
// the maximum double is 1.7E+308, 250^7
// does not exceed the maximum					 

/*
int main( int argc, char *argv[])
{
	
	Clique array[3];



    fprintf(stdout,"3333");
 //    Clique *clique1 = clique_init_6(0, 1,2,3,4,5,6);
	// Clique *clique2 = clique_init_6(0, 2,3,4,5,6,7);
 //    Clique *clique3 = clique_init_6(0, 3,4,5,6,7,8);
 	
 	Clique *clique1 = clique_init_7(0, 1,2,3,4,5,6,7);
	Clique *clique2 = clique_init_7(0, 2,3,4,5,6,7,8);
    Clique *clique3 = clique_init_7(0, 3,4,5,6,7,8,9);


	array[0] = *clique1;
	array[1]= *clique2;
   	array[2]= *clique3;  
     

	int *g =(int*)malloc(64*sizeof(int*));
	int counter = search_7(array, 3, 1,2);
     //int counter = search_6(array,3, g, 8, 3,  6,  10);
     fprintf(stdout," counter = %d \n ",counter);
//int *g =(int*)malloc(64*sizeof(int*));
//int counter = list_search_6(list, g, 10, 10,  8,  100);
//fprintf(stdout," counter = %d \n ",counter);
    // Clique *clique3;
    // clique3 = (Clique*) malloc(sizeof(Clique));
    // clique_init_6(clique3, 0, 3,4,5,6,7,8);
    // list_add(list,clique3); 
    return(0); 
} */


int search_7(int i, int j){

	int counter = 0;
	int p = 0;
	while(p<cache_7.length){
		int *nodeArray = cache_7.array[p].clique_node;
		bool inclique_i = false;
		bool inclique_j = false;
		int r;

		for(r = 0; r < 7; r++){
			if(nodeArray[r] == i)
				inclique_i=true;
			if(nodeArray[r]==j)
				inclique_j=true;
		}		
		if( inclique_j && inclique_i)
			counter++;
	    p++;
	}
	return counter;
}


int search_6( int *g, int g_size, int i, int j, int stop){
	JRB root = make_jrb();   //   comment 1  by yanhong
	int counter = 0, p = 0, t, q = 0;
	int searchEnd = binary_search_first(cache_6.array, 0, cache_6.length-1, j);
  int key_array[7];
	bool out_index_added = false;
	double key = 0; 
	long radix = 1; 
	while(p<searchEnd && counter <= stop){
		if(cache_6.array[p].clique_node[5]>=i){
			int *nodeArray = cache_6.array[p].clique_node;
			bool inclique_i = false;
			bool inclique_j = false;
			int out_index;
			int r = 0;
	
			for(; r < 6; r++){
				if(nodeArray[r] == i)
					inclique_i = true;
				if(nodeArray[r] == j)
					inclique_j = true;
			}
			int clique_color = cache_6.array[p].color;
			if(inclique_i!=inclique_j){
				if(inclique_j)
					out_index = i;
				else out_index = j;
				for( r = 0; r < 6; r++){
					if(nodeArray[r]<out_index){
        				if(g[nodeArray[r]*g_size+out_index]!=clique_color)
							break;
					} else{
            			if(g[out_index*g_size+nodeArray[r]]!=clique_color)
              				break;
          			}          
				}
				if(r==6){
				//	counter ++;
					q=0;
					out_index_added = false;
					key = 0; radix = 1; 
					for(t = 0; t < 7; t ++) {
						if(q >= 6 || (out_index < nodeArray[q] 
												&& !out_index_added)) {
							key_array[t] = out_index;
							out_index_added = true;
						}
						else key_array[t] = nodeArray[q ++];
					}
					// get a key
					MAKE_KEY(key_array, key, radix)
					if(!jrb_find_dbl(root, key)) {
						counter ++;
						jrb_insert_dbl(root, key, new_jval_v(NULL));
					//	for(p = 0; p < 7; p ++)
					//	printf("%d\t", key_array[p]);
					//printf("\n");
					}
        }
			}
		}
		p++;
	}

	if(counter > stop)
    	return(-1);

	return counter;
}

int binary_search_first(Clique6 *array, int start, int end, int target){
	int s = start;
	int e = end;

	while(s<=end && e>=start && s<e){

		int half = (s+e)/2;
		if(array[half].clique_node[0]<=target){
			s=half+1;
		}
		else{
			e = half;
		}
	}
	//printf("end of binary_search_first\n");
	return s;
}


	




















