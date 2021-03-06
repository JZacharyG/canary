#include "setgraph.h"
#include <string.h>
#include <stdio.h>
#include "debug.h"
#define BIAS6 63
#define SMALLN 62

// parses graph6 into a setgraph.  Assumes that you have not called allocate_setgraph on g yet, and requires you to call free_setgraph when you are done with it. 
const char* g62setgraph(const char g6[], setgraph* g)
{
	int i, j, k, x, n;
	
	if (*g6 == ':')
	{
		fprintf(stderr, "Cannot read sparse6 format\n");
		exit(1);
	}
	n = *g6++ - BIAS6;
	if (n > SMALLN)
	{
		for (i = 0, n = 0; i < 3; ++i)
		{
			n = (n << 6) | (*g6++ - BIAS6);
		}
	}
	
	if (n >= MAXNV)
	{
		fprintf(stderr, "Graph of size %d is larger than maximum size %d\n", g->nv, MAXNV);
		exit(1);
	}
	allocate_setgraph(g, n);
	
	for (i = 1, k = 1; i < n; ++i)
	{
		for (j = 0; j < i; ++j)
		{
			if (!--k) {
				k = 6;
				if (*g6 < '?' || *g6 > '~')
				{
					if (*g6 == '\0')
						fprintf(stderr, "Error : Graph6 string ended prematurely\n");
					else
						fprintf(stderr, "Error : Character '%c' not valid in Graph6\n", *g6);
					exit(1);
				}
				x = *(g6++) - BIAS6;
			}
			if ((x >> 5) & 1)
				add_edge(g, i, j);
			x <<= 1;
		}
	}
	if (x & 63)
		fprintf(stderr, "Warning: unread bits in Graph6 string\n");
	return g6; // in case you wanted to process whatever came afterwards
}

// Expects a string in path list format, and anything in the string after the graph is complete is ignored.
//path list: with vertices labeled sequentially with lowercase letters starting at 'a', the graph should be given as a space delimited list of walks, with each walk being specified as a list of lowercase letters.  After the last path should be a '.' to indicate that the graph is complete.
void gpl2setgraph(const char gpl[], setgraph* g)
{
	const char* c=gpl;
	int nv=0;
	while (*c!='.')
	{
		if ('a' <= *c && *c <= 'z')
		{
			int v = (int)((*c)-'a')+1;
			if (v>nv) nv = v;
		}
		else if (*c == '\0')
		{
			fprintf(stderr, "gpl2setgraph: pretty abrupt ending. Perhaps you forgot a '.'?\n");
			exit(1);
		}
		else if (*c != ' ')
		{
			fprintf(stderr, "gpl2setgraph: unexpected character %c\n", *c);
			exit(1);
		}
		++c;
	}
	
	allocate_setgraph(g, nv);
	
	c=gpl;
	while (*c != '.')
	{
		if ('a' <= *c && *c <= 'z')
		{
			int prev = (int)(*c-'a');
			++c;
			while ('a' <= *c && *c <= 'z')
			{
				int v = (int)((*c)-'a');
				if (v == prev)
				{
					fprintf(stderr, "gpl2setgraph: No loops, please.\n");
					exit(1);
				}
				add_edge(g, prev, v);
				prev=v;
				++c;
			}
		}
		
		while (*c == ' ')
			++c;
	}
}

// FIX ME.  Make this style more consistent with the rest of the code.
void print_adjacency_list(const setgraph * const g)
{
	int i, j;
	for (i = 0; i < g->nv; ++i)
	{
		printf("%d :", i);
		for (j = 0; j < g->nv; ++j)
		{
			if (setget(g->nbhd[i], j))
			{
				printf(" %d", j);
			}
		}
		printf("\n");
	}
}

// assumes that i2v is has length at least g->nv
void order_vertices(const setgraph* const g, vertex* const i2v)
{
	bitset remaining = fullset(g->nv);
	for (int i=0; i < g->nv; ++i)
	{
		vertex v, bestv;
		unsigned int score, bestscore; // nv*number of earlier adjacencies + deg
		
		db_print("looking for i=%d/%d, bitset to choose from: %llo\n", i, g->nv, remaining);
		fflush(stdout);
		
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
			db_print("\tchoosing %d\n", bestv);
			i2v[i]=bestv;
			setremoveeq(remaining, bestv);
		}
		else {assert(0);}
	}
}

void rev_order_vertices(const setgraph* const g, vertex* const i2v)
{
	bitset remaining = fullset(g->nv);
	for (int i=0; i < g->nv; ++i)
	{
		vertex v, bestv;
		unsigned int score, bestscore; // nv*number of earlier adjacencies + deg
		if (first(remaining, &v))
		{
			bestv = v;
			bestscore = -(g->nv * setsize(setminus(g->nbhd[v], remaining)) + setsize(g->nbhd[v]));
			while (next(remaining, &v, v))
			{
				score = -(g->nv * setsize(setminus(g->nbhd[v], remaining)) + setsize(g->nbhd[v]));
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


// assumes that newg is (at least) as large as g and has been allocated.  this is... probably suboptimal.  FIX ME?
void relabel_into(const setgraph* const g, setgraph* const newg, vertex* const i2v)
{
	assert(newg->nv >= g->nv);
	newg->nv = g->nv;
	for (vertex i = 0; i < newg->nv; ++i)
	{
		newg->nbhd[i] = emptyset;
		for (vertex j = 0; j < newg->nv; ++j)
			if (setget(g->nbhd[i2v[i]],i2v[j]))
				setaddeq(newg->nbhd[i],j);
	}
}

#ifndef EXCLUDE_NAUTY
void setgraph2nautygraph(const setgraph* const sg, graph* ng)
{
	int m=SETWORDSNEEDED(sg->nv);
	memset(ng, 0, sg->nv*m*sizeof(setword));
	vertex v,w;
	for (v=0; v < sg->nv; ++v)
	{
		if (next(sg->nbhd[v], &w, v)) do
		{
			ADDONEEDGE(ng,v,w,m);
		} while (next(sg->nbhd[v], &w, w));
	}
}
#endif
