#ifndef SETGRAPH_H
#define SETGRAPH_H
#include <stdlib.h>
#include <assert.h>
#define MAXNV 32
#include "set.h"

typedef struct
{
	int nv;
	set* nbhd;
} setgraph;

static inline void allocate_setgraph(setgraph* s, int nv)
{
	s->nv = nv;
	s->nbhd = malloc(nv*sizeof(set));
	for (int i = 0; i < nv; ++i)
		s->nbhd[i] = emptyset;
}

static inline void free_setgraph(setgraph* s)
{
	free(s->nbhd);
}

static inline void add_edge(setgraph* s, int v1, int v2)
{
	assert(0 <= v1 && v1 < s->nv);
	assert(0 <= v2 && v2 < s->nv);
	setaddeq(s->nbhd[v1], v2);
	setaddeq(s->nbhd[v2], v1);
}

void g62setgraph(const char g6[], setgraph* g);
void print_adjacency_list(const setgraph* const g);


#endif