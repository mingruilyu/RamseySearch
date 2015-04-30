#include "list.h"
#ifndef CLIQUE_COUNT_H
#define CLIQUE_COUNT_H

int CliqueCount(int *g, int gsize);
int CliqueCountFast(int *g, int gsize, int stop_7, long *clique_6);
int CliqueCountUseCache(int *g, int gsize, int i, int j, int stop, List *cache_6, List *cache_7);
int CliqueCountCreateCache(int *g, int gsize, List *cache_6, List *cache_7);
#endif

