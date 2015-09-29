/* Canary
 * Testing things for minors.
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

//#define DEBUG 1
#define DEBUG 0
#define STATS 1
//#define STATS 0
#include <stdio.h>
#include "debug.h"
#include "set.h"
#include <setjmp.h>

#define max(i,j) (((i)>(j))?(i):(j))
#define min(i,j) (((i)<(j))?(i):(j))

#define NONE -1

typedef int vertex;

typedef struct path path;

typedef struct
{
	path* p;
	int oldc1, oldc2;
	set oldassigned1, oldassigned2, oldsemi1, oldsemi2; // FIX ME? We could recompute instead of storing.
} mod;

// c1 and c2 should not be used at all on an incomplete path; they are only set as a path is completed
// 
struct path
{
	vertex h1, h2; // the two branch sets that this path connects.  the search starts from h1's branch set and proceeds towards h2's.
	
	// the indices of the last vertex known to be part of the branch set 1
	// and first known to be part of branch set 2.
	int c1, c2;
	
//	int i2Gv[MAXNV];
	
	int len; // the next available position in the path.  actually one more than the number of vertices in the path, because we leave a blank at the 0th element
	
	set i2psofar[MAXNV];
	set i2nbhdsofar[MAXNV]; // the neighborhood around vertices in the path up to this point.  might include other vertices in the path, vertices already assigned or semi-assigned.
	int gv2i[MAXNV];
	path* next;
};

struct hdata
{
	int hnv;
	vertex firsthv;
	vertex hv2next[MAXNV];
	vertex hv2symm[MAXNV];
};

typedef struct
{
	set gv2nbhd[MAXNV]; // just to store the graph
	int gnv;
	hdata hd;
	
	mod mods[MAXNV]; // a stack of all modifications to cutoffs.  Every mod should fix at least one additional vertex, so the number is limited to the number of vertices in G.
	int numMods;
	
	set free;
	set hv2assigned[MAXNV];
	set hv2semiassigned[MAXNV];
	set hv2allowed[MAXNV+1];
	path* hv2firstpath[MAXNV];
	
	path* gv2p[MAXNV]; // vertices in G
	
	jmp_buf victory; // where we go if we find a minor.
} searchData;

void initialize_hdata(hdata* hd, const setgraph* const h)
{
	hd->hnv = h->nv;
	hd->firsthv = 0;
	for (int hv=0; hv<hd->hnv; ++hv)
	{
		hd->hv2next[hv] = hv+1;
		hd->hv2symm[hv] = hd->hnv;
	}
	hd->hv2next[hd->hnv-1] = NONE;
}

void initialize_searchData(searchData* d, const setgraph* const g, const setgraph* const h)
{
	 // just to store the graph
	d->gnv = g->nv;
	for (vertex gv = 0; gv < d->gnv; ++gv)
	{
		d->gv2nbhd[gv] = g->nbhd[gv];
		d->gv2p[gv] = NULL;
	}
	set sofar = emptyset;
	for (vertex hv = 0; hv < d->hd.hnv; ++hv)
	{
		d->hv2assigned[hv] = emptyset;
		d->hv2semiassigned[hv] = emptyset;
		int h2;
		path** pp = &d->hv2firstpath[hv];
		set nbhd = setintsct(h->nbhd[hv],sofar);
		if (first(nbhd, &h2)) do
		{
			*pp = malloc(sizeof(path));
 			(**pp).h1 = hv;
 			(**pp).h2 = h2;
  			for (int gv = 0; gv < d->gnv; ++gv)
 				(**pp).gv2i[gv] = NONE;
 			pp = &((**pp).next);
		} while (next(nbhd, &h2, h2));
		*pp = NULL;
		setaddeq(sofar, hv);	
	}
	d->hv2allowed[d->hd.hnv] = fullset(d->gnv);
	d->free = fullset(d->gnv);
	
	d->numMods = 0;
}

void push_v(searchData* d, path* p, vertex gv) // :)
{
	assert(p != NULL && d != NULL);
	assert(d->gv2p[gv] == NULL && setget(d->free, gv));
	setremoveeq(d->free, gv);
	p->gv2i[gv] = p->len;
	d->gv2p[gv] = p;
	p->i2psofar[p->len] = setadd(p->i2psofar[p->len-1], gv);
	p->i2nbhdsofar[p->len] = setunion(p->i2nbhdsofar[p->len-1],d->gv2nbhd[gv]);
	++p->len;
}

void pop_v(searchData* d, path* p, vertex gv) // :)
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
void fix_BS(searchData* d, vertex gv, vertex hv) // ...
{
	assert(d != NULL);
	path* p = d->gv2p[gv];
	assert(p != NULL);
	d->mods[d->numMods].p = p;
	d->mods[d->numMods].oldc1 = p->c1;
	d->mods[d->numMods].oldc2 = p->c2;
	d->mods[d->numMods].oldassigned1 = d->hv2assigned[p->h1];
	d->mods[d->numMods].oldassigned2 = d->hv2assigned[p->h2];
	d->mods[d->numMods].oldsemi1 = d->hv2semiassigned[p->h1];
	d->mods[d->numMods].oldsemi2 = d->hv2semiassigned[p->h2];
	
	++d->numMods;
	set s;
	if (p->h1 == hv)
	{
		p->c1 = p->gv2i[gv];
		s = p->i2psofar[p->c1];
		setunioneq(d->hv2assigned[p->h1], s);
		setminuseq(d->hv2semiassigned[p->h1], s);
		setminuseq(d->hv2semiassigned[p->h2], s);
	}
	else
	{
		assert(p->h2 == hv);
		p->c2 = p->gv2i[gv];
		s = setminus(p->i2psofar[p->len-1], p->i2psofar[p->c2-1]);
		setunioneq(d->hv2assigned[p->h2], s);
		setminuseq(d->hv2semiassigned[p->h1], s);
		setminuseq(d->hv2semiassigned[p->h2], s);
	}
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2semiassigned[p->h1])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h2],d->hv2semiassigned[p->h2])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2assigned[p->h2])));
	assert(p->c1 < p->c2);
}

// force a vertex to be in the other branch set, if possible.
// adjusts the cutoffs of the relevant path and adds a mod to the stack.
void fix_BS_not(searchData* d, vertex gv, vertex hv) // :)
{
	assert(d != NULL);
	path* p = d->gv2p[gv];
	assert(p != NULL);
	assert(setget(d->hv2semiassigned[p->h1], gv));
	assert(setget(d->hv2semiassigned[p->h2], gv));
	assert(!setget(d->hv2assigned[p->h1], gv));
	assert(!setget(d->hv2assigned[p->h2], gv));
	d->mods[d->numMods].p = p;
	d->mods[d->numMods].oldc1 = p->c1;
	d->mods[d->numMods].oldc2 = p->c2;
	d->mods[d->numMods].oldassigned1 = d->hv2assigned[p->h1];
	d->mods[d->numMods].oldassigned2 = d->hv2assigned[p->h2];
	d->mods[d->numMods].oldsemi1 = d->hv2semiassigned[p->h1];
	d->mods[d->numMods].oldsemi2 = d->hv2semiassigned[p->h2];
	
	++d->numMods;
	set s;
	if (p->h2 == hv)
	{
		p->c1 = p->gv2i[gv];
		s = p->i2psofar[p->c1];
		setunioneq(d->hv2assigned[p->h1], s);
		setminuseq(d->hv2semiassigned[p->h1], s);
		setminuseq(d->hv2semiassigned[p->h2], s);
	}
	else
	{
		assert(p->h1 == hv);
		p->c2 = p->gv2i[gv];
		s = setminus(p->i2psofar[p->len-1], p->i2psofar[p->c2-1]);
		setunioneq(d->hv2assigned[p->h2], s);
		setminuseq(d->hv2semiassigned[p->h1], s);
		setminuseq(d->hv2semiassigned[p->h2], s);
	}
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2semiassigned[p->h1])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h2],d->hv2semiassigned[p->h2])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2assigned[p->h2])));
	assert(p->c1 < p->c2);
}

void undo_last_mod(searchData* d) // :) 
{
	assert(d!=NULL && d->numMods > 0);
	
	--d->numMods;
	mod* m = &d->mods[d->numMods];
	m->p->c1 = m->oldc1;
	m->p->c2 = m->oldc2;
	d->hv2assigned[m->p->h1] = m->oldassigned1;
	d->hv2assigned[m->p->h2] = m->oldassigned2;
	d->hv2semiassigned[m->p->h1] = m->oldsemi1;
	d->hv2semiassigned[m->p->h2] = m->oldsemi2;
}

void undo_mods(searchData* d, int n) // :)
{
	while (d->numMods > n)
		undo_last_mod(d);
}

void finish_path(searchData* d, path* p, int c1, int c2) // :)
{
	p->c1 = c1;
	p->c2 = min(c2, p->len); // nb: these might not point to a vertex of the path
	
	setunioneq(d->hv2assigned[p->h1], p->i2psofar[p->c1]);
	setunioneq(d->hv2assigned[p->h2], setminus(p->i2psofar[p->len-1],p->i2psofar[p->c2-1]));
	setunioneq(d->hv2semiassigned[p->h1], setminus(p->i2psofar[p->c2-1], p->i2psofar[p->c1]));
	setunioneq(d->hv2semiassigned[p->h2], setminus(p->i2psofar[p->c2-1], p->i2psofar[p->c1]));
	
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2semiassigned[p->h1])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h2],d->hv2semiassigned[p->h2])));
	assert(!setnonempty(setintsct(d->hv2assigned[p->h1],d->hv2assigned[p->h2])));
	assert(p->c1 < p->c2);
}

void unfinish_path(searchData* d, path* p) // :)
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
set path_after(searchData* d, vertex gv, vertex hv) // :)
{
	path* p = d->gv2p[gv];
	assert(p != NULL && (p->h1 == hv || p->h2 == hv));
	int i = p->gv2i[gv];
	return (p->h1 == hv)?
		(setminus(p->i2psofar[p->len-1], p->i2psofar[i])):
		(p->i2psofar[i-1]);
}

void build_BS(searchData* d, vertex hv);
void build_path(searchData* d, path* p, set bsnbhd);
void add_to_path(searchData* d, path* p, vertex gv, int c1, int c2,  set bsnbhd);
void build_next(searchData* d, path* p, set bsnbhd);

// start building the specified path
void build_path(searchData* d, path* p, set bsnbhd)
{
	/* initialize things */
	
	assert(p != NULL);
	int firstMod = d->numMods;
	p->c1=0; p->c2=1; p->len=1;
	
	// if assigned h1 is adjacent to assigned h2
	if (setnonempty(setintsct(bsnbhd, d->hv2assigned[p->h2])))
	{
		build_next(d, p, bsnbhd);
		//undo_mods(d,firstMod);
		return;
	}
	
	// if assigned h1 is adjacent to semiassigned h2
	vertex nbr;
	set s = setintsct(bsnbhd, d->hv2semiassigned[p->h2]);
	if (first(s, &nbr)) do
	{
		setminuseq(s,path_after(d, nbr, p->h2));
	} while (next(s, &nbr, nbr));
	if (first(s, &nbr)) do
	{
		fix_BS(d, nbr, p->h2);
		build_next(d, p, bsnbhd);
		undo_last_mod(d);
		fix_BS_not(d, nbr, p->h2);
	} while (next(s, &nbr, nbr));
		
	

	
	
	
	// if semiassigned h1 is adjacent to assigned h2
	// it sure would be nice if we had the nbhd of h2 available, but alas.
	vertex v; // a vertex, assigned or semiassigned to h1
	set nbhd; // vertices w such that v is the first vertex in v's path that w
	          // is adjacent to.
	path* vp; // v's path
	set newbsnbhd;
	s = d->hv2semiassigned[p->h1];
	if (first(s, &v)) do
	{
		vp = d->gv2p[v];
		nbhd = setminus(vp->i2psofar[vp->gv2i[v]], vp->i2psofar[vp->gv2i[v]-1]);
		setintscteq(nbhd, d->hv2assigned[p->h2]);
		if (setnonempty(nbhd)) // This only needs to happen once per v.
		{
			fix_BS(d, v, p->h1);
			newbsnbhd = setunion(bsnbhd, vp->i2psofar[vp->gv2i[v]]);
			build_next(d,p,bsnbhd);
			undo_last_mod(d);
			fix_BS_not(d, v, p->h1);
		}
	} while (next(s, &v, v));
	
	
	
	// if semiassigned h1 is adjacent to semiassigned h2
	// s should still hold the semiassigned vertices
	if (first(s, &v)) do
	{
		vp = d->gv2p[v];
		nbhd = setminus(vp->i2psofar[vp->gv2i[v]], vp->i2psofar[vp->gv2i[v]-1]);
		setintscteq(nbhd, d->hv2semiassigned[p->h2]);
		if (first(nbhd, &nbr)) do
		{
			setminuseq(nbhd, path_after(d, nbr, p->h2));
		} while(next(nbhd, &nbr, nbr));
		if (first(nbhd, &nbr))
		{
			fix_BS(d, v, p->h1);
			newbsnbhd = setunion(bsnbhd, vp->i2psofar[vp->gv2i[v]]);
			do
			{
				fix_BS(d, nbr, p->h2);
				build_next(d, p, newbsnbhd);
				undo_last_mod(d);
				
				// sadly, we can't conclude anything if these fail.  It tells us that at
				// least one of the pair is assigned to the other end of the path, but since
				// we don't know which and don't have a good way of keeping track of this
				// information...
			} while (next(nbhd, &nbr, nbr));
			undo_last_mod(d);
		}
	} while (next(s, &v, v));
	
	
	
	// paths that start from something assigned to h1
	s = setintsct(bsnbhd, d->free);
	if (first(s, &nbr)) do
	{
		add_to_path(d, p, nbr, 0, MAXNV, bsnbhd);
	} while (next(s, &nbr, nbr));
	
	
	
	// paths that start from something semiassigned to h1
	s = d->hv2semiassigned[p->h1];
	if (first(s, &v)) do
	{
		vp = d->gv2p[v];
		nbhd = setminus(vp->i2psofar[vp->gv2i[v]], vp->i2psofar[vp->gv2i[v]-1]);
		setintscteq(nbhd, d->free);
		if (first(nbhd, &nbr))
		{
			fix_BS(d, v, p->h1);
			newbsnbhd = setunion(bsnbhd, vp->i2nbhdsofar[vp->gv2i[v]]);
			do
			{
				add_to_path(d, p, nbr, 0, MAXNV, newbsnbhd);
			} while (next(nbhd, &nbr, nbr));
			undo_last_mod(d);
		}
	} while (next(s, &v, v));
	
	undo_mods(d, firstMod);
}

// assumes that lastGv is allowed to be added to the given path.
// Adds it, continues the search, and returns if it fails to build this into a complete model for the minor (jumps if successful).
void add_to_path(searchData* d, path* p, vertex gv, int c1, int c2,  set bsnbhd)
{
	// if we are using vertices that are not allowed in one or both of the branch sets, adjust the cutoffs accordingly.
	assert(c1 < c2);
	
	// This is to respect the hv2allowed sets.
	// Note: len is the position where gv will be
	if (!setget(d->hv2allowed[p->h2],gv))
		c1 = p->len;
	if (!setget(d->hv2allowed[p->h1],gv))
		c2 = min(c2, p->len);
	if (c1 >= c2)
		return;
	
	push_v(d, p, gv);
	int firstMod = d->numMods;
	
	
	set nbhd = setminus(setminus(setminus(d->gv2nbhd[gv], p->i2psofar[p->len-1]), p->i2nbhdsofar[p->len-2]), bsnbhd);
	if setnonempty(setintsct(nbhd, d->hv2assigned[p->h2]))
	{
		finish_path(d, p, c1, c2);
		build_next(d, p, bsnbhd);
		unfinish_path(d, p);
		pop_v(d, p, gv);
		return; // if that failed then this path is hopeless.
	}
	
	int nbr;
	
	set semicomplete = setintsct(nbhd, d->hv2semiassigned[p->h2]); // make sure that we aren't marking things in this path as assigned/semiassigned yet
//	we should find the last vertex adjacent to this one that can be part of Hv for each adjacent path of Hv.
	// When we get rid of these, we introduce errors because if we have 
	// (h1) -- a -- b -- (h2), and we fail to use b, then we will mark a and b as being part of h1, but we don't update the semicomplete set, so we will try to use a anyway.  BAM!  Data structure has been broken.
	if (first(semicomplete, &nbr)) do
	{
		setminuseq(semicomplete, path_after(d, nbr, p->h2));
	} while (next(semicomplete, &nbr, nbr));
	
	if (first(semicomplete, &nbr)) do
	{
		fix_BS(d, nbr, p->h2);
		
		finish_path(d,p,c1,c2);
		build_next(d,p,bsnbhd);
		unfinish_path(d,p);
		
		undo_last_mod(d);
		fix_BS_not(d, nbr, p->h2);
	} while (next(semicomplete, &nbr, nbr));
	
	set possible_next = setminus(setintsct(nbhd, d->free), p->i2nbhdsofar[p->len-2]);
	
	if (first(possible_next, &nbr)) do
	{
		add_to_path(d, p, nbr, c1, c2, bsnbhd);
	} while (next(possible_next, &nbr, nbr));
	
	undo_mods(d, firstMod);
	
	pop_v(d, p, gv);
}

void build_BS(searchData* d, vertex hv)
{
	d->hv2allowed[hv] = setintsct(d->free,d->hv2allowed[d->hd.hv2symm[hv]]);
	d->hv2assigned[hv] = d->hv2semiassigned[hv] = emptyset;
	vertex gv;
	if (d->hv2firstpath[hv] == NULL)
	{
		if (first(d->hv2allowed[hv], &gv)) do
		{
			setremoveeq(d->free, gv);
			setaddeq(d->hv2assigned[hv], gv);
		
			build_BS(d, d->hd.hv2next[hv]);
			
			setaddeq(d->free, gv);
			d->hv2assigned[hv] = d->hv2semiassigned[hv] = emptyset;
			setremoveeq(d->hv2allowed[hv], gv);
		} while(next(d->hv2allowed[hv], &gv, gv));
	}
	else
	{
		if (first(d->hv2allowed[hv], &gv)) do
		{
			setremoveeq(d->free, gv);
			setaddeq(d->hv2assigned[hv], gv);
			
			build_path(d, d->hv2firstpath[hv], d->gv2nbhd[gv]);
			
			setaddeq(d->free, gv);
			d->hv2assigned[hv] = d->hv2semiassigned[hv] = emptyset;
			setremoveeq(d->hv2allowed[hv], gv);
		} while(next(d->hv2allowed[hv], &gv, gv));
	}
}

// build the next path or branch set as appropriate.
void build_next(searchData* d, path* p, set bsnbhd)
{
	assert(d!=NULL && p!=NULL);
	if (p->next != NULL) // more paths needed
		build_path(d, p->next, bsnbhd);
	else if (d->hd.hv2next[p->h1] != NONE)
		build_BS(d, d->hd.hv2next[p->h1]);
	else
		longjmp(d->victory, true); // yay!
}

void freepath(path* p)
{
	if (p != NULL)
		freepath(p->next);
	free(p);
}

bool has_minor(const setgraph* const g, const setgraph* const h)
{
	// setup
	bool has = false;
	searchData d;
	initialize_hdata(&d.hd, h);
	initialize_searchData(&d, g, h);
	
	if (setjmp(d.victory))
	{
		has = true; // win!
		// prep the branch sets
	}
	else
	{
		// run search
		build_BS(&d, d.hd.firsthv);
	}
	
	// clean up
	
	for (int i = 0; i < d.hd.hnv; ++i)
		freepath(d.hv2firstpath[i]);
	
	return has;
}