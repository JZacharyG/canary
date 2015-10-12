#ifndef SET_H
#define SET_H

// implementation of a bit-set.  Does not carry along the size of the universe,
// but assumes (and should maintain) that any bits that are not possible are 0

#include <stdint.h>
#include <stdbool.h>
#if MAXNV <= 32
typedef uint32_t bitset;
#elif MAXNV <= 64
typedef uint64_t bitset;
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

#define setmask(n)       (~((bitset)-1 << (n)))
#define emptyset         ((bitset)0)
#define fullset(n)       setmask((n))

#define setunion(s1,s2)  ((s1) | (s2))
#define setintsct(s1,s2) ((s1) & (s2))
#define setminus(s1,s2)  ((s1) & ~(s2))
#define setget(s,i)      (((s) >> (i)) & (bitset)1)
#define singleton(i)     ((bitset)1 << (i))
#define setadd(s,i)      ((s) |  singleton(i))
#define settoggle(s,i)   ((s) ^  singleton(i))
#define setremove(s,i)   ((s) & ~singleton(i))


//#define setcomp(s,n)     (~(s) & setmask((n)))
#define setcomp(s,n)     (s ^ setmask((n)))

#define setunioneq(s1,s2)  ((s1) |= (s2))
#define setintscteq(s1,s2) ((s1) &= (s2))
#define setminuseq(s1,s2)  ((s1) &= ~(s2))
#define setaddeq(s,i)      ((s) |= (bitset)1 << (i))
#define settoggleeq(s,i)   ((s) ^= (bitset)1 << (i))
#define setremoveeq(s,i)   ((s) &= ~((bitset)1 << (i)))
//#define setcompeq(s,n)     ((s) = setcomp((s),(n)))
#define setcompeq(s,n)     ((s) ^= setmask((n)))

#define setnonempty(s)     (s)
#define issubset(s1,s2)    (!((~(s1))&(s2)))


#if MAXNV <= 32
#define setsize(s)         (__builtin_popcountl(s))
#elif MAXNV <= 64
#define setsize(s)         (__builtin_popcountll(s))
#endif

static inline bool first(bitset s, int* i)
{
	if (setnonempty(s))
	{
#if MAXNV <= 32
		*i = __builtin_ctzl(s);
#elif MAXNV <= 64
		*i = __builtin_ctzll(s);
#endif
		return true;
	}
	return false;
}

static inline bool next(bitset s, int* i, int p)
{
	// clear bits upto and including p, then find the first bitset bit.
	return first(s & ((bitset)(-1) << (p+1)), i);
}

// bool first(bitset s, int* i)
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
// bool first_binary_search(bitset s, int* i)
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


//http://graphics.stanford.edu/~seander/bithacks.html#NextBitPermutation
// next subbitset of the same size
//bitset v; // current permutation of bits
//bitset w; // next permutation of bits

//bitset t = v | (v - 1); // t gets v's least significant 0 bits bitset to 1
// Next bitset to 1 the most significant bit to change,
// bitset to 0 the least significant ones, and add the necessary 1 bits.
//w = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));

#endif