/* Canary.
 * Tests things for minors.
 * 
 * This file provides a function that, given two simple graphs G and H, tests
 * if H is a minor of G.
 *
 * Cheers,
 * J. Zachary G.
 *
 * Version 1.0
 * September 28, 2015
 */




#include "canary.h"
#ifndef DEBUG
#define DEBUG 0
#endif
//#define STATS 1
#define STATS 0
#include <stdio.h>
#include "debug.h"
#include <setjmp.h>
#include <alloca.h>

#define max(i,j) (((i)>(j))?(i):(j))
#define min(i,j) (((i)<(j))?(i):(j))

#define NONE -1

typedef struct path path;

typedef struct
{
	vertex hva, hvb;
	bitset oldassigned_a, oldsemi_a, oldsemi_b, oldbsnbhd_a;
	// in fixing a vertex, we are assigning some vertices to one branch set (hva), and removing them from the semiassigned set of another (hvb)
} mod;

struct path
{
	vertex h1, h2; // the two branch sets that this path connects.  the search starts from h1's branch set and proceeds towards h2's.
	
	int len; // the next available position in the path.  actually one more than the number of vertices in the path, because we leave a blank at the 0th element
	
	bitset i2psofar[MAXNV];
	bitset i2nbhdsofar[MAXNV]; // the neighborhood around vertices in the path up to this point.  might include other vertices in the path, vertices already assigned or semi-assigned.
	int gv2i[MAXNV];
	path* next;
};

struct hdata
{
	int hnv;
	vertex firsthv;
	vertex hv2next[MAXNV]; // the order in which to carry out the search for branch sets.
	vertex hv2symm[MAXNV]; // To take advantage of automorphisms in H, each vertex can point to an earlier one, in which case it's anchors are chosen to be smaller than the anchor of that vertex.  hv2anchor[hnv] will always be gnv, so those without symmetry will point there.
	int hv2numsymm[MAXNV]; // the number of vertices that will restrict themselves (directly or indirectly) to this vertex's rejected anchors.
};

typedef struct
{
	setgraph* g;
	
	hdata hd;
	
	mod mods[MAXNV]; // a stack of all modifications to cutoffs.  Every mod should fix at least one additional vertex, so the number is limited to the number of vertices in G.
	int numMods;
	
	bitset free;
	bitset hv2assigned[MAXNV];
	bitset hv2semiassigned[MAXNV];
	bitset hv2bsnbhd[MAXNV];
	vertex hv2anchor[MAXNV+1];
	path* hv2firstpath[MAXNV];
	
	path* gv2p[MAXNV]; // vertices in G
	
	jmp_buf victory; // where we go if we find a minor.
} searchData;


void ensure_valid(searchData* d) { if (DEBUG)
{
	for (vertex h1=0; h1 < d->hd.hnv; ++h1)
	{
		if (setnonempty(setintsct(d->hv2assigned[h1], d->hv2semiassigned[h1])))
			{ printf("%d's assigned and semiassigned overlap overlap\n", h1); assert(0); }
		for (vertex h2=h1+1; h2 < d->hd.hnv; ++h2)
		{
			
			if (setnonempty(setintsct(d->hv2assigned[h1], d->hv2assigned[h2])))
				{ printf("%d and %d overlap\n", h1, h2); assert(0); }
			for (vertex h3=h2+1; h3 < d->hd.hnv; ++h3)
			{
				assert(!setnonempty(setintsct(setintsct(d->hv2semiassigned[h1], d->hv2semiassigned[h2]), d->hv2semiassigned[h3])));
			}
		}
	}
	
	for (vertex gv=0; gv < d->g->nv; ++gv)
	{
		path* p=d->gv2p[gv];
		if (setget(d->free,gv) && (p!=NULL))
			{ printf("Inconsistency: free %s %d, but gv2p %s\n",setget(d->free,gv)?"has":"doesn't have", gv, (p==NULL)?"is NULL":"isn't NULL"); assert(0); }

		
		if (p!=NULL)
		{
			assert(p->i2psofar[0]==emptyset);
			assert(p->i2nbhdsofar[0]==emptyset);
			int i=p->gv2i[gv];
			assert(i>0);
			assert(i<p->len);
			assert(p->i2psofar[i] == setadd(p->i2psofar[i-1], gv));
			assert(p->i2nbhdsofar[i] == setunion(p->i2nbhdsofar[i-1], d->g->nbhd[gv]));
		}
	}
	//printf("passed!\n");
}}

void print_search_data(searchData* d, vertex upto_hv, path* upto_p){ if (DEBUG)
{
	vertex hv = d->hd.firsthv;
	while (hv != NONE)
	{
		printf("%d: ", hv);
		print_set(d->hv2assigned[hv]);
		if (setnonempty(d->hv2semiassigned[hv]))
		{
			printf(" (");
			print_set(d->hv2semiassigned[hv]);
			printf(")");
		}
		printf("\n");
		printf("\tanchor: %d",d->hv2anchor[hv]);
		printf("\n");
		if (hv == upto_hv)
			break;
		hv = d->hd.hv2next[hv];
	}
	printf("free: ");
	print_set(d->free);
	printf("\n\n");
	
	if (upto_p == NULL)
		return;
	hv=d->hd.firsthv;
	while (hv != NONE)
	{
		path* p = d->hv2firstpath[hv];
		while (p != NULL)
		{
			printf("[%d] ", p->h1);
			for (int i=1;i < p->len; ++i)
			{
				printf(" ");
				vertex v;
				first(setminus(p->i2psofar[i], p->i2psofar[i-1]),&v);
				if (setget(d->hv2assigned[p->h2],v))
					printf("(");
					printf("%d", v);
				if (setget(d->hv2assigned[p->h1],v))
					printf(")");
			}
			printf(" [%d]   (len:%d)\n", p->h2, p->len);
			if (p == upto_p)
				break;
			p = p->next;
		}
		if (hv == upto_hv)
			break;
		hv = d->hd.hv2next[hv];
	}
}}


#ifndef EXCLUDE_NAUTY
void findHSymmetries(hdata* d, setgraph* h)
{
	int n=d->hnv;
	int m=SETWORDSNEEDED(n);
	graph* nh = alloca(n*m*sizeof(setword));
	setgraph2nautygraph(h, nh);
	int labels[MAXNV];
	int ptn[MAXNV];
	int orbits[MAXNV];
	
	DEFAULTOPTIONS_GRAPH(options);
	options.defaultptn = 0;
	statsblk stats;
	
	for (vertex v=0; v < n; ++v)
		d->hv2symm[v]=n;
	vertex v,w;
	int i,j;
	for (i=0, v=d->firsthv; i<n; ++i, v=d->hv2next[v])
	{
		for (j=0, w=d->firsthv; j<n; ++j, w=d->hv2next[w])
		{
			labels[j] = w;
			ptn[j] = (j < i)?0:1;
		}
		ptn[n-1] = 0;
		densenauty(nh, labels, ptn, orbits, &options, &stats, m, d->hnv, NULL);
		d->hv2numsymm[v] = 0;
		for (w=d->hv2next[v]; w != NONE; w=d->hv2next[w])
		{
			if (orbits[w] == orbits[v])
			{
				d->hv2symm[w] = v;
				++d->hv2numsymm[v];
			}
		}
	}
}
#endif

void initialize_hdata(hdata* hd, setgraph* h)
{
	hd->hnv = h->nv;
	
	vertex* i2hv = malloc(h->nv*sizeof(vertex));
	order_vertices(h, i2hv);
// 	for (int i = 0; i<h->nv; ++i) // to disable the sorting of H's vertices
// 		i2hv[i]=i;
	for (int i = 0; i<h->nv; ++i)
		db_print("%d ", i2hv[i]);
	db_print("\n");
	
	hd->firsthv = i2hv[0];
	for (int i=0; i < hd->hnv-1; ++i)
	{
		hd->hv2next[i2hv[i]] = i2hv[i+1];
	}
	hd->hv2next[i2hv[hd->hnv-1]] = NONE;
	
	db_print("hv ordering");
	vertex hv = hd->firsthv;
	do
	{
		db_print("%d ", hv);
		hv = hd->hv2next[hv];
	} while (hv != NONE);
	db_print("\n");
	
#ifndef EXCLUDE_NAUTY
	findHSymmetries(hd,h);
	
	db_print("hv2symm :");
	for (hv=0; hv<h->nv; ++hv)
	{
		db_print(" %d", hd->hv2symm[hv]);
	}
	db_print("\n");
	db_print("numsymm :");
	for (hv=0; hv<h->nv; ++hv)
	{
		db_print(" %d", hd->hv2numsymm[hv]);
	}
	db_print("\n");
// 	hv = hd->firsthv;
// 	do
// 	{
// 		db_print("%d ", hd->hv2symm[hv]);
// 		hv = hd->hv2next[hv];
// 	} while (hv != NONE);
// 	db_print("\n");
#else
	for (vertex hv=0; hv < hd->hnv; ++hv)
	{
		hd->hv2symm[hv] = hd->hnv;
		hd->hv2numsymm[hv] = 0;
	}
#endif
	free(i2hv);
}

void initialize_searchData(searchData* restrict d, setgraph* g, setgraph* h)
{
	// just to store the graph
	d->g = g;
	for (vertex gv = 0; gv < g->nv; ++gv)
	{
		d->gv2p[gv] = NULL;
	}
	bitset sofar = emptyset;
	vertex hv = d->hd.firsthv;
	do
	{
		d->hv2assigned[hv] = emptyset;
		d->hv2semiassigned[hv] = emptyset;
		d->hv2bsnbhd[hv] = emptyset;
		int h2;
		path** pp = &d->hv2firstpath[hv];
		bitset nbhd = setintsct(h->nbhd[hv],sofar);
		if (first(nbhd, &h2)) do
		{
			*pp = malloc(sizeof(path));
			assert(*pp!=NULL);
 			(**pp).h1 = hv;
 			(**pp).h2 = h2;
  			for (int gv = 0; gv < g->nv; ++gv)
 				(**pp).gv2i[gv] = NONE;
 			(**pp).i2psofar[0] = emptyset;
 			(**pp).i2nbhdsofar[0] = emptyset;
 			pp = &((**pp).next);
		} while (next(nbhd, &h2, h2));
		*pp = NULL;
		setaddeq(sofar, hv);
		hv = d->hd.hv2next[hv];
	} while (hv != NONE);
	d->hv2anchor[d->hd.hnv] = g->nv;
	d->free = fullset(g->nv);
	
	d->numMods = 0;
}

void push_v(searchData* restrict d, path* p, vertex gv) // :)
{
	assert(p != NULL && d != NULL);
	assert(d->gv2p[gv] == NULL && setget(d->free, gv));
	setremoveeq(d->free, gv);
	p->gv2i[gv] = p->len;
	d->gv2p[gv] = p;
	p->i2psofar[p->len] = setadd(p->i2psofar[p->len-1], gv);
	p->i2nbhdsofar[p->len] = setunion(p->i2nbhdsofar[p->len-1],d->g->nbhd[gv]);
	++p->len;
}

void pop_v(searchData* restrict d, path* p, vertex gv) // :)
{
	assert(p != NULL && d != NULL);
	int i = --p->len;
	assert(p->gv2i[gv] == i);
	assert(d->gv2p[gv] == p);
	setaddeq(d->free,gv);
	p->gv2i[gv] = NONE;
	d->gv2p[gv] = NULL;
	// I don't think there's any need to clear the psofar and nbhdsofar entries
}

// force a vertex to be in a branch set, if possible.
// adjusts the cutoffs of the relevant path and adds a mod to the stack.
void fix_BS(searchData* restrict d, vertex gv, vertex hv) // ...
{
	assert(d != NULL);
	path* p = d->gv2p[gv];
	assert(p != NULL);
	assert(setget(d->hv2semiassigned[p->h1], gv));
	assert(setget(d->hv2semiassigned[p->h2], gv));
	assert(!setget(d->hv2assigned[p->h1], gv));
	assert(!setget(d->hv2assigned[p->h2], gv));
	
	mod* m = &d->mods[d->numMods];
	++d->numMods;
	
	bitset diff, diffnbhd; // the set of vertices to be added to the hva's branch set.
	if (p->h1 == hv)
	{
		m->hva = p->h1; m->hvb = p->h2;
		diff = p->i2psofar[p->gv2i[gv]];
		diffnbhd = p->i2nbhdsofar[p->gv2i[gv]];
	}
	else
	{
		assert(p->h2 == hv);
		m->hva = p->h2; m->hvb = p->h1;
		diff = setminus(p->i2psofar[p->len-1], p->i2psofar[p->gv2i[gv]-1]);
		diffnbhd = emptyset;
		vertex v;
		if (first(diff, &v)) do
		{
			setunioneq(diffnbhd, d->g->nbhd[v]);
		} while (next(diff, &v, v));
	}

	m->oldassigned_a = d->hv2assigned[m->hva];
	m->oldsemi_a = d->hv2semiassigned[m->hva];
	m->oldsemi_b = d->hv2semiassigned[m->hvb];
	m->oldbsnbhd_a = d->hv2bsnbhd[m->hva];
	
	setunioneq(d->hv2assigned[m->hva], diff);
	setminuseq(d->hv2semiassigned[m->hva], diff);
	setminuseq(d->hv2semiassigned[m->hvb], diff);
	setminuseq(d->hv2bsnbhd[m->hva], diffnbhd);
	
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2semiassigned[p->h1])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h2],d->hv2semiassigned[p->h2])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2assigned[p->h2])));
}

// force a vertex to be in the other branch set, if possible.
// adjusts the cutoffs of the relevant path and adds a mod to the stack.
void fix_BS_not(searchData* restrict d, vertex gv, vertex hv) // :)
{
	assert(d != NULL);
	path* p = d->gv2p[gv];
	assert(p != NULL);
	assert(setget(d->hv2semiassigned[p->h1], gv));
	assert(setget(d->hv2semiassigned[p->h2], gv));
	assert(!setget(d->hv2assigned[p->h1], gv));
	assert(!setget(d->hv2assigned[p->h2], gv));
	
	mod* m = &d->mods[d->numMods];
	++d->numMods;
	
	bitset diff, diffnbhd; // the set of vertices to be added to the hva's branch set.
	if (p->h2 == hv)
	{
		m->hva = p->h1; m->hvb = p->h2;
		diff = p->i2psofar[p->gv2i[gv]];
		diffnbhd = p->i2nbhdsofar[p->gv2i[gv]];
	}
	else
	{
		assert(p->h1 == hv);
		m->hva = p->h2; m->hvb = p->h1;
		diff = setminus(p->i2psofar[p->len-1], p->i2psofar[p->gv2i[gv]-1]);
		diffnbhd = emptyset;
		vertex v;
		if (first(diff, &v)) do
		{
			setunioneq(diffnbhd, d->g->nbhd[v]);
		} while (next(diff, &v, v));
	}

	m->oldassigned_a = d->hv2assigned[m->hva];
	m->oldsemi_a = d->hv2semiassigned[m->hva];
	m->oldsemi_b = d->hv2semiassigned[m->hvb];
	m->oldbsnbhd_a = d->hv2bsnbhd[m->hva];
	
	setunioneq(d->hv2assigned[m->hva], diff);
	setminuseq(d->hv2semiassigned[m->hva], diff);
	setminuseq(d->hv2semiassigned[m->hvb], diff);
	setminuseq(d->hv2bsnbhd[m->hva], diffnbhd);
	
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2semiassigned[p->h1])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h2],d->hv2semiassigned[p->h2])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2assigned[p->h2])));
	ensure_valid(d);
}

void undo_last_mod(searchData* restrict d) // :) 
{
	assert(d!=NULL && d->numMods > 0);
	
	--d->numMods;
	mod* m = &d->mods[d->numMods];
	d->hv2assigned[m->hva] = m->oldassigned_a;
	d->hv2semiassigned[m->hva] = m->oldsemi_a;
	d->hv2semiassigned[m->hvb] = m->oldsemi_b;
	d->hv2bsnbhd[m->hva] = m->oldbsnbhd_a;
}

void undo_mods(searchData* restrict d, int n) // :)
{
	while (d->numMods > n)
		undo_last_mod(d);
}

void finish_path(searchData* restrict d, path* p, int c1, int c2) // :)
{
	c2 = min(c2, p->len); // nb: this might not point to a vertex of the path
	
	setunioneq(d->hv2assigned[p->h1], p->i2psofar[c1]);
	setunioneq(d->hv2assigned[p->h2], setminus(p->i2psofar[p->len-1],p->i2psofar[c2-1]));
	setunioneq(d->hv2semiassigned[p->h1], setminus(p->i2psofar[c2-1], p->i2psofar[c1]));
	setunioneq(d->hv2semiassigned[p->h2], setminus(p->i2psofar[c2-1], p->i2psofar[c1]));
	
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2semiassigned[p->h1])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h2],d->hv2semiassigned[p->h2])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2assigned[p->h2])));
	assert(c1 < c2);
}

void unfinish_path(searchData* restrict d, path* p) // :)
{
	// just need to remove the vertices in the path from the relevant sets.
	// Note that we maintain d->free as we go, so we only clear from the relevant branch sets
	setminuseq(d->hv2assigned[p->h1], p->i2psofar[p->len-1]);
	setminuseq(d->hv2assigned[p->h2], p->i2psofar[p->len-1]);
	setminuseq(d->hv2semiassigned[p->h1], p->i2psofar[p->len-1]);
	setminuseq(d->hv2semiassigned[p->h2], p->i2psofar[p->len-1]);
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2semiassigned[p->h1])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h2],d->hv2semiassigned[p->h2])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2assigned[p->h2])));
}

// returns the portion of the path containing gv that occurs after (and not including) gv, treating the hv side of the path as the front.
bitset path_after(searchData* restrict d, vertex gv, vertex hv) // :)
{
	path* p = d->gv2p[gv];
	assert(p != NULL && (p->h1 == hv || p->h2 == hv));
	int i = p->gv2i[gv];
	return (p->h1 == hv)?
		(setminus(p->i2psofar[p->len-1], p->i2psofar[i])):
		(p->i2psofar[i-1]);
}

void build_BS(searchData* restrict d, vertex hv);
void build_path(searchData* restrict d, path* p, bitset bsnbhd);
void add_to_path(searchData* restrict d, path* p, vertex gv, int c1, int c2,  bitset bsnbhd);
void build_next(searchData* restrict d, path* p, bitset bsnbhd);

// start building the specified path
long long build_path_count = 0;
void build_path(searchData* restrict d, path* p, bitset bsnbhd)
{
	stat_increment(build_path_count);
	/* initialize things */
	
	assert(p != NULL);
	int firstMod = d->numMods;
	p->len=1;
	
	// if assigned h1 is adjacent to assigned h2
	if (setnonempty(setintsct(bsnbhd, d->hv2assigned[p->h2])))
	{
		int enteringNumMods = d->numMods;
		build_next(d, p, bsnbhd);
		assert(enteringNumMods == d->numMods);
		//undo_mods(d,firstMod);
		ensure_valid(d);
		return;
	}
	
	// if assigned h1 is adjacent to semiassigned h2
	vertex nbr;
	bitset s = setintsct(bsnbhd, d->hv2semiassigned[p->h2]);
	ensure_valid(d);
	if (first(s, &nbr)) do
	{
		setminuseq(s,path_after(d, nbr, p->h2));
	} while (next(s, &nbr, nbr));
	if (first(s, &nbr)) do
	{
		fix_BS(d, nbr, p->h2);
		int enteringNumMods = d->numMods;
		build_next(d, p, bsnbhd);
		assert(enteringNumMods == d->numMods);
		undo_last_mod(d);
		fix_BS_not(d, nbr, p->h2);
		ensure_valid(d);
	} while (next(s, &nbr, nbr));
	
	
	// FIX ME: Probably keep track of bsnbhd for each hv, to facilitate some of these computations
	
	
	// if semiassigned h1 is adjacent to assigned h2
	// it sure would be nice if we had the nbhd of h2 available, but alas.
	vertex v; // a vertex, assigned or semiassigned to h1
	bitset nbhd; // vertices w such that v is the first vertex in v's path that w
	          // is adjacent to.
	path* vp; // v's path
	bitset newbsnbhd;
	s = setintsct(d->hv2semiassigned[p->h1], d->hv2bsnbhd[p->h2]);
	if (first(s, &v)) do
	{
		setminuseq(s,path_after(d, v, p->h1));
	} while (next(s,&v,v));
	if (first(s, &v)) do
	{
		vp = d->gv2p[v];
		fix_BS(d, v, p->h1);
		newbsnbhd = setunion(bsnbhd, vp->i2nbhdsofar[vp->gv2i[v]]);
		int enteringNumMods = d->numMods;
		build_next(d,p,newbsnbhd);
		assert(enteringNumMods == d->numMods);
		undo_last_mod(d);
		fix_BS_not(d, v, p->h1);
		setintscteq(s, d->hv2semiassigned[p->h1]);
		ensure_valid(d);
	} while (next(s, &v, v));
	
	// if semiassigned h1 is adjacent to semiassigned h2
	s = d->hv2semiassigned[p->h1];
	ensure_valid(d);
	if (first(s, &v)) do
	{
		vp = d->gv2p[v];
		nbhd = setminus(vp->i2nbhdsofar[vp->gv2i[v]], vp->i2nbhdsofar[vp->gv2i[v]-1]);
		setintscteq(nbhd, d->hv2semiassigned[p->h2]);
		if (first(nbhd, &nbr)) do
		{
			setminuseq(nbhd, path_after(d, nbr, p->h2));
		} while(next(nbhd, &nbr, nbr));
		if (first(nbhd, &nbr))
		{
			fix_BS(d, v, p->h1);
			newbsnbhd = setunion(bsnbhd, vp->i2nbhdsofar[vp->gv2i[v]]);
			do
			{
				fix_BS(d, nbr, p->h2);
				int enteringNumMods = d->numMods;
				build_next(d, p, newbsnbhd);
				assert(enteringNumMods == d->numMods);
				undo_last_mod(d);
				ensure_valid(d);
				// sadly, we can't conclude anything if these fail.  It tells us that at
				// least one of the pair is assigned to the other end of the path, but since
				// we don't know which and don't have a good way of keeping track of this
				// information...
			} while (next(nbhd, &nbr, nbr));
			undo_last_mod(d);
			ensure_valid(d);
		}
	} while (next(s, &v, v));
	
	// paths that start from something assigned to h1
	s = setintsct(bsnbhd, d->free);
	ensure_valid(d);
	if (first(s, &nbr)) do
	{
		int enteringNumMods = d->numMods;
		add_to_path(d, p, nbr, 0, MAXNV, bsnbhd);
		assert(enteringNumMods == d->numMods);
		ensure_valid(d);
	} while (next(s, &nbr, nbr));
	
	// paths that start from something semiassigned to h1
	s = d->hv2semiassigned[p->h1];
	if (first(s, &v)) do
	{
		vp = d->gv2p[v];
		nbhd = setminus(vp->i2nbhdsofar[vp->gv2i[v]], vp->i2nbhdsofar[vp->gv2i[v]-1]);
		setintscteq(nbhd, d->free);
		setminuseq(nbhd,bsnbhd);
		if (first(nbhd, &nbr))
		{
			ensure_valid(d);
			fix_BS(d, v, p->h1);
			
			newbsnbhd = setunion(bsnbhd, vp->i2nbhdsofar[vp->gv2i[v]]);
			do
			{
				int enteringNumMods = d->numMods;
				add_to_path(d, p, nbr, 0, MAXNV, newbsnbhd);
				assert(enteringNumMods == d->numMods);
				ensure_valid(d);
			} while (next(nbhd, &nbr, nbr));
			undo_last_mod(d);
		}
	} while (next(s, &v, v));
	
	undo_mods(d, firstMod);
}

// assumes that gv is 'allowed' to be added to the given path, in that it is adjacent to the previous vertex on its path and it is not immediately redundant.
// Adds it, continues the search, and returns if it fails to build this into a complete model for the minor (jumps if successful).
long long add_to_path_count = 0;
void add_to_path(searchData* restrict d, path* p, vertex gv, int c1, int c2,  bitset bsnbhd)
{
	stat_increment(add_to_path_count);
	// if we are using vertices that are not allowed in one or both of the branch sets, adjust the cutoffs accordingly.
	assert(c1 < c2);
	
	// Note: len is the position where gv will be
	if (d->hv2anchor[p->h2] > gv)
		c1 = p->len;
	if (d->hv2anchor[p->h1] > gv)
		c2 = min(c2, p->len);
	if (c1 >= c2)
		return;
	
	push_v(d, p, gv);
	int firstMod = d->numMods;
	
	
	if (setnonempty(setintsct(d->g->nbhd[gv], d->hv2assigned[p->h2])))
	{
		ensure_valid(d);
		finish_path(d, p, c1, c2);
		int enteringNumMods = d->numMods;
		build_next(d, p, setunion(bsnbhd, p->i2nbhdsofar[c1]));
		assert(enteringNumMods == d->numMods);
		unfinish_path(d, p);
		pop_v(d, p, gv);
		ensure_valid(d);
		return; // if that failed then this path is hopeless.
	}
	assert(!setnonempty(setintsct(d->g->nbhd[gv], d->hv2assigned[p->h2])));
	int nbr;
	
// 	This seems to make things quite a bit slower, despite doing a little bit of pruning.  Also, this isn't going to mess with the current path, right?  I don't think so...
// 	bitset semih1nbrs = setintsct(d->g->nbhd[gv], d->hv2semiassigned[p->h1]);
// 	if (first(semih1nbrs, &nbr)) do
// 	{
// 		fix_BS_not(d, nbr, p->h1);
// 		setintscteq(semih1nbrs,d->hv2semiassigned[p->h1]);
// 	} while (next(semih1nbrs, &nbr, nbr));
	
	bitset semicomplete = setintsct(d->g->nbhd[gv], d->hv2semiassigned[p->h2]); // make sure that we aren't marking things in this path as assigned/semiassigned yet
//	we should find the last vertex adjacent to this one that can be part of Hv for each adjacent path of Hv.
	// When we get rid of these, we introduce errors because if we have 
	// (h1) -- a -- b -- (h2), and we fail to use b, then we will mark a and b as being part of h1, but we don't update the semicomplete set, so we will try to use a anyway.  BAM!  Data structure has been broken.
	if (first(semicomplete, &nbr)) do
	{
		setminuseq(semicomplete, path_after(d, nbr, p->h2));
	} while (next(semicomplete, &nbr, nbr));
	
	if (first(semicomplete, &nbr)) do
	{
		ensure_valid(d);
		fix_BS(d, nbr, p->h2);
		
		finish_path(d,p,c1,c2);
		int enteringNumMods = d->numMods;
		build_next(d,p,setunion(bsnbhd, p->i2nbhdsofar[c1]));
		assert(enteringNumMods == d->numMods);
		unfinish_path(d,p);
		
		undo_last_mod(d);
		fix_BS_not(d, nbr, p->h2);
		ensure_valid(d);
	} while (next(semicomplete, &nbr, nbr));
	
	//bitset possible_next = setminus(d->g->nbhd[gv], p->i2psofar[p->len-1]);
	bitset possible_next = setminus(setminus(setminus(d->g->nbhd[gv], p->i2psofar[p->len-1]), p->i2nbhdsofar[p->len-2]), bsnbhd);
	possible_next = setintsct(possible_next, d->free);

	if (first(possible_next, &nbr)) do
	{
		ensure_valid(d);
		int enteringNumMods = d->numMods;
		add_to_path(d, p, nbr, c1, c2, bsnbhd);
		assert(enteringNumMods == d->numMods);
		ensure_valid(d);
	} while (next(possible_next, &nbr, nbr));
	
	undo_mods(d, firstMod);
	
	pop_v(d, p, gv);
}

long long build_BS_count = 0;
void build_BS(searchData* restrict d, vertex hv)
{
	stat_increment(build_BS_count);
	db_print("Attempting to find a branch set for %d\n", hv);
	vertex* anchorp = &d->hv2anchor[hv];
	print_search_data(d, hv, NULL);
	
// 	bitset s = setrange(d->free, d->hd.hv2numsymm[hv], d->hv2anchor[d->hd.hv2symm[hv]]);
	bitset s = setintsct(d->free, setmask(d->hv2anchor[d->hd.hv2symm[hv]]));
	for (int retain = d->hd.hv2numsymm[hv]; retain; --retain)
		s=setremovefirst(s);
	
	if (DEBUG)
	{
		printf("%d <= free < %d ?: ", d->hd.hv2numsymm[hv], d->hv2anchor[d->hd.hv2symm[hv]]);
		print_set(s);
		printf("\n");
	}
	if (d->hv2firstpath[hv] == NULL)
	{
		while (first(s, anchorp))
		{
			setremoveeq(d->free, *anchorp);
			d->hv2assigned[hv] = singleton(*anchorp);
			d->hv2semiassigned[hv] = emptyset;
			
			build_BS(d, d->hd.hv2next[hv]);
			
			setaddeq(d->free, *anchorp);
			setremoveeq(s, *anchorp);
		}
	}
	else
	{
		while (first(s, anchorp))
		{
			setremoveeq(d->free, *anchorp);
			d->hv2assigned[hv] = singleton(*anchorp);
			d->hv2semiassigned[hv] = emptyset;
			
			build_path(d, d->hv2firstpath[hv], d->g->nbhd[*anchorp]);
			
			setaddeq(d->free, *anchorp);
			setremoveeq(s, *anchorp);
		}
	}
	d->hv2assigned[hv] = d->hv2semiassigned[hv] = emptyset;
}

// build the next path or branch set as appropriate.
void build_next(searchData* restrict d, path* p, bitset bsnbhd)
{
	assert(d!=NULL && p!=NULL);
	if (p->next != NULL) // more paths needed
		build_path(d, p->next, bsnbhd);
	else if (d->hd.hv2next[p->h1] != NONE)
	{
		d->hv2bsnbhd[p->h1] = bsnbhd;
		build_BS(d, d->hd.hv2next[p->h1]);
	}
	else
		longjmp(d->victory, 1); // yay!
}
// if we were using alloca, we wouldn't need this...
void freepath(path* p)
{
	if (p != NULL)
		freepath(p->next);
	free(p);
}

int has_minor(setgraph* g, setgraph* h, bitset* hv2bs)
{
	// setup
	int has = 0;
	// FIX ME? where should I be selecting the ordering of the vertices?  here?  outside?  inside?
	vertex* i2gv = malloc(g->nv * sizeof(vertex));
	setgraph sorted_g;
	allocate_setgraph(&sorted_g, g->nv);
	order_vertices(g, i2gv);
	relabel_into(g, &sorted_g, i2gv);
//	print_adjacency_list(&sorted_g);
	searchData d;
	initialize_hdata(&d.hd, h);
	initialize_searchData(&d, &sorted_g, h);
//	initialize_searchData(&d, g, h);
	
	if (setjmp(d.victory))
	{
		has = 1; // win!
		if (hv2bs != NULL)
		{
			// return a model of the minor
			bitset assigned = emptyset;
			for (vertex hv=0; hv < h->nv; ++hv)
			{
				assert(!setnonempty(setintsct(assigned, d.hv2assigned[hv])));
				hv2bs[hv] = setunion(d.hv2assigned[hv], setminus(d.hv2semiassigned[hv], assigned));
				setunioneq(assigned, hv2bs[hv]);
				
				// those are done in the reordered G, but we want to return a model that uses the original G
				hv2bs[hv] = relabel(hv2bs[hv], i2gv);
			}
		}
	}
	else
	{
		build_BS(&d, d.hd.firsthv);
	}
	
	ensure_valid(&d);
	
	for (int i = 0; i < d.hd.hnv; ++i)
		freepath(d.hv2firstpath[i]);
	free(i2gv);
	free_setgraph(&sorted_g);
	stats_print("bbs:%lld\nbp :%lld\na2p:%lld\n",build_BS_count, build_path_count, add_to_path_count);
	return has;
}

int is_minor(setgraph* g, setgraph* h, bitset* hv2bs)
{
	for (vertex hv = 0; hv < h->nv; ++hv)
	{
		// check that the branch set is connected
		bitset queue = getfirst(hv2bs[hv]), comp = emptyset, nbhd = emptyset;
		int i;
		if (first(queue, &i)) do
		{
			setremoveeq(queue, i);
			setaddeq(comp,i);
			setunioneq(queue, setminus(setintsct(g->nbhd[i], hv2bs[hv]), comp));
			setunioneq(nbhd, g->nbhd[i]);
		} while (first(queue, &i));
		else
			return 0;
		if (comp != hv2bs[hv])
			return 0;
		
		// check that it has the right adjacencies
		if (first(h->nbhd[hv], &i)) do
		{
			if (setisempty(setintsct(nbhd, hv2bs[i])))
				return 0;
		} while (next(h->nbhd[hv], &i, i));
	}
	return 1;
}
