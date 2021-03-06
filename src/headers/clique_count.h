#include "list.h"
#ifndef CLIQUE_COUNT_H
#define CLIQUE_COUNT_H

int CliqueCount(int *g, int gsize);
int CliqueCountFast(int *g, int gsize, int stop, long* clique_6);
int CliqueCountUseCache(int *g, int gsize, int i, int j, int stop);
int CliqueCountCreateCache(int *g, int gsize);
#endif

