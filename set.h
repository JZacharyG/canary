#ifndef SET_H
#define SET_H

#include <stdint.h>
#include <stdbool.h>
#if MAXNV <= 32
typedef uint32_t set;
#elif MAXNV <= 64
typedef uint64_t set;
#endif

// look-up table for the location of the least significant (right-most)
// non-zero bit in a byte. More accurately (because 0 -> 8), the number
// of trailing 0s in a byte.
// const int firstbit[] =
// {
// 	8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
// 	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0
// }

#define setmask(n)       (~(-1 << (n)))
#define emptyset         (0)
#define fullset(n)       setmask((n))

#define setunion(s1,s2)  ((s1) | (s2))
#define setintsct(s1,s2) ((s1) & (s2))
#define setminus(s1,s2)  ((s1) & ~(s2))
#define setget(s,i)      (((s) >> (i)) & 1)
#define setadd(s,i)      ((s) | 1 << (i))
#define settoggle(s,i)   ((s) ^ 1 << (i))
#define setremove(s,i)   ((s) & ~(1 << (i)))
//#define setcomp(s,n)     (~(s) & setmask((n)))
#define setcomp(s,n)     (s ^ setmask((n)))

#define setunioneq(s1,s2)  ((s1) |= (s2))
#define setintscteq(s1,s2) ((s1) &= (s2))
#define setminuseq(s1,s2)  ((s1) &= ~(s2))
#define setaddeq(s,i)      ((s) |= 1 << (i))
#define settoggleeq(s,i)   ((s) ^= 1 << (i))
#define setremoveeq(s,i)   ((s) &= ~(1 << (i)))
//#define setcompeq(s,n)     ((s) = setcomp((s),(n)))
#define setcompeq(s,n)     ((s) ^= setmask((n)))

#define setnonempty(s)     (s)

#if MAXNV <= 32
static inline bool first(set s, int* i)
{
	if (setnonempty(s))
	{
		*i = __builtin_ctzl(s);
		return true;
	}
	return false;
}
#elif MAXNV <= 64
static inline bool first(set s, int* i)
{
	if (setnonempty(s))
	{
		*i = __builtin_ctzll(s);
		return true;
	}
	return false;
}
#endif

static inline bool next(set s, int* i, int p)
{
	// clear bits upto and including p, then find the first set bit.
	return first(s & (-1 << (p+1)), i);
}

// bool first(set s, int* i)
// {
// 	if (s & 0xFF)
// 		*i = firstbit[s & 0xFF], return true;
// 	if (s & 0xFF00)
// 		*i = firstbit[s >> 8 & 0xFF], return true;
// 	if (s & 0xFF0000)
// 		*i = firstbit[s >> 16 & 0xFF], return true;
// 	if (s & 0xFF000000)
// 		*i = firstbit[s >> 24 & 0xFF], return true;
// 	if (s & 0xFF00000000)
// 		*i = firstbit[s >> 32 & 0xFF], return true;
// 	if (s & 0xFF0000000000)
// 		*i = firstbit[s >> 40 & 0xFF], return true;
// 	if (s & 0xFF000000000000)
// 		*i = firstbit[s >> 48 & 0xFF], return true;
// 	if (s & 0xFF00000000000000)
// 		*i = firstbit[s >> 56 & 0xFF], return true;
// 	return false;
// }
// 
// bool first_binary_search(set s, int* i)
// {
// 	if (setnonempty(s))
// 	{
// 		if (s & 0x00000000FFFFFFFF)
// 			if (s & 0x000000000000FFFF)
// 				if (s & 0x00000000000000FF)
// 					*i = firstbit[s & 0xFF];
// 				else
// 					*i = firstbit[s >> 8 & 0xFF];
// 			else
// 				if (s & 0x0000000000FF0000)
// 					*i = firstbit[s >> 16 & 0xFF];
// 				else
// 					*i = firstbit[s >> 24 & 0xFF];
// 		else
// 			if (s & 0x0000FFFF00000000)
// 				if (s & 0x000000FF00000000)
// 					*i = firstbit[s >> 32 & 0xFF];
// 				else
// 					*i = firstbit[s >> 40 & 0xFF];
// 			else
// 				if (s & 0x00FF000000000000)
// 					*i = firstbit[s >> 48 & 0xFF];
// 				else
// 					*i = firstbit[s >> 56 & 0xFF];
// 		return true;
// 	}
// 	return false;
// }

#endif