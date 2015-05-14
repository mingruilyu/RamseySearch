#include "clique_count.h"
#include "graph.h"
#include "stdio.h"

extern Clique6list cache_6;
extern Clique7list cache_7;

int main() {
	int* g;
	int gsize, i, j, count_1, count_2;
	ReadGraph("ce", &g, &gsize);
	PrintGraph(g, gsize);
	long clique_6;	
//	
//	List *cache_6 = list_init(6);
//	List *cache_7 = list_init(7);
 //   Clique6 cache_6[1000000];
//    int i;
//    for( i= 0; i < 1000000; i++){
//        cache_6[i]=(Clique *)malloc(sizeof(Clique));
//    }
  //  Clique7 cache_7[1000];
//    for( i= 0; i < 1000; i++){
//        cache_7[i]=(Clique *)malloc(sizeof(Clique));
//    }
    
    
    
    

	CliqueCountCreateCache(g, gsize);
	printf("# of clique_6:\t%d\n", cache_6.length);
	printf("# of clique_7:\t%d\n", CliqueCount(g, gsize));
//	int a, b, c, d, e, f, h;
//	a = 71, b = 72, c = 77, d = 90, e = 96, f = 101, h = 104;
//	printf("%d\t%d\t%d\t%d\t%d\t\n", g[a * gsize + b], g[a * gsize + c], g[a * gsize + d], g[a * gsize + e], g[a * gsize + f]);
//	printf("%d\t%d\t%d\t%d\t\n", g[b * gsize + c], g[b * gsize + d], g[b * gsize + e], g[b * gsize + f]);
//	printf("%d\t%d\t%d\t\n", g[c * gsize + d], g[c * gsize + e], g[c * gsize + f]);
//	printf("%d\t%d\t\n", g[d * gsize + e], g[d * gsize + f]);
//	printf("%d\t\n", g[e * gsize + f]);
//	
//	printf("%d\t%d\t%d\t%d\t%d\t%d\t", g[a * gsize + h], g[h * gsize + b], g[h * gsize + c], g[h * gsize + d], g[h * gsize + e], g[h * gsize + f]);
	printf("======================================\n");
	printf("======================================\n");
//	print_list(cache_6);
//	print_list(cache_7);
	for(i = 0; i < gsize; i ++) {
	//i = 20;
		for(j = i + 1; j < gsize; j ++) {
			//printf("Preflip (%d, %d)\n", i, j);
			g[i * gsize + j] = 1 - g[i * gsize + j];
			
			count_1 = CliqueCountFast(g, gsize, 1000, &clique_6);
			//printf("# of clique_7 by normal:\t%d\n", count_1);
//			printf("# of clique_7 by fast:\t%d\n", count_1);
			
			count_2 = CliqueCountUseCache(g, gsize, i, j, 10000);
			//printf("# of clique_7 by cache:\t%d\n", count_2);
			printf("count = %d\n", count_2);
			if(count_1 != count_2) { 
				printf("# of clique_7 by normal:\t%d\n", count_1);
				printf("# of clique_7 by cache:\t%d\n", count_2);
				printf("edge(%d, %d)\n", i, j);
			}
			g[i * gsize + j] = 1 - g[i * gsize + j];
		}
	}
	i = 45, j = 57;
	printf("Preflip (%d, %d)\n", i, j);
	g[i * gsize + j] = 1 - g[i * gsize + j];
	count_1 = CliqueCount(g, gsize);
	printf("# of clique_7 by normal:\t%d\n", count_1);
	count_2 = CliqueCountUseCache(g, gsize, i, j, 10000);
	printf("# of clique_7 by cache:\t%d\n", count_2);
/*	i = 64, j = 79;
	printf("Preflip (%d, %d)\n", i, j);
	g[i * gsize + j] = 1 - g[i * gsize + j];
	i = 50, j = 70;
	printf("Preflip (%d, %d)\n", i, j);
	g[i * gsize + j] = 1 - g[i * gsize + j];
	i = 70, j = 79;
	printf("Preflip (%d, %d)\n", i, j);
	g[i * gsize + j] = 1 - g[i * gsize + j];*/


	printf("\n\n");
	free(g);

	return 0;
}
