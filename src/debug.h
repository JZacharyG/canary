#ifndef DEBUG_H
#define DEBUG_H

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef STATS
#define STATS 1
#endif

// This code (slightly modified) was suggested in http://stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing
// The if vs #ifdef is so that it is always included (so that any debug statements are seen by the compiler, and must stay up to date).  It should be optimized out if you aren't debugging.
// The do ... while is to ensure that it behaves the way you expect, particularly if there are if ... else ... blocks nearby
//#define db_print(...) \
//		do { if (DEBUG) fprintf(stderr, __VA_ARGS__); } while (0)

// But I kind of like this more?  I think it has the same effect.
#define db_print(...) { if (DEBUG) fprintf(stderr, __VA_ARGS__); }

#define stat_increment(s) { if (STATS) ++(s); }
#define stats_print(...) { if (STATS) fprintf(stderr, __VA_ARGS__); }

#if !DEBUG
	#define NDEBUG // disable assert statements
#endif

#include <assert.h>

#endif
