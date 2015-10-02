#include "setgraph.h"
#include <string.h>
#include <stdio.h>
#define BIAS6 63
#define SMALLN 62

void g62setgraph(const char g6[], setgraph* g) {
  int i, j, k, x;

  if (*g6 == ':') {
    fprintf(stderr, "Cannot read sparse6 format\n");
    exit(1);
  }

  allocate_setgraph(g, *g6++ - BIAS6);
  if (g->nv > SMALLN) {
    for (i = 0, g->nv = 0; i < 3; ++i) {
      g->nv = (g->nv << 6) | (*g6++ -BIAS6);
    }
  }

  if (g->nv >= MAXNV) {
    fprintf(stderr, "Graph of size %d is larger than maximum size %d\n", g->nv, MAXNV);
    exit(1);
  }

  for (i = 1, k = 1; i < g->nv; ++i) {
    for (j = 0; j < i; ++j) {
      if (!--k) {
        k = 6;
        x = *(g6++) - BIAS6;
      }
      if ((x >> 5) & 1)
      	add_edge(g, i, j);
      x <<= 1;
    }
  }
}

void print_adjacency_list(const setgraph * const g) {
  int i, j;
  for (i = 0; i < g->nv; ++i) {
    printf("  %d :", i);
    for (j = 0; j < g->nv; ++j) {
      if (setget(g->nbhd[i], j)) {
        printf(" %d", j);
      }
    }
    printf(";\n");
  }
}

// assumes that i2v is has length at least g->nv
void order_vertices(const setgraph* const g, vertex* const i2v)
{
	set remaining = fullset(g->nv);
	for (int i=0; i < g->nv; ++i)
	{
		vertex v, bestv;
		unsigned int score, bestscore; // nv*number of earlier adjacencies + deg
		if (first(remaining, &v))
		{
			bestv = v;
			bestscore = g->nv * setsize(setminus(g->nbhd[v], remaining)) + setsize(g->nbhd[v]);
			while (next(remaining, &v, v))
			{
				score = g->nv * setsize(setminus(g->nbhd[v], remaining)) + setsize(g->nbhd[v]);
				if (score > bestscore)
				{
					bestv = v;
					bestscore = score;
				}
			}
			i2v[i]=bestv;
			setremoveeq(remaining, bestv);
		}
		else {assert(0);}
	}
}