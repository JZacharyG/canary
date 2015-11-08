#ifndef DEBUG_H
#define DEBUG_H

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef STATS
#define STATS 1
#endif

// The if (as opposed to #ifdef) is so that it is always included (so that any
// debug statements are seen by the compiler, and must stay up to date).  It
// should be optimized out if you aren't debugging.
#define db_print(...) { if (DEBUG) fprintf(stderr, __VA_ARGS__); }

#define stat_increment(s) { if (STATS) ++(s); }
#define stats_print(...) { if (STATS) fprintf(stderr, __VA_ARGS__); }

#if !DEBUG
	#define NDEBUG // disable assert statements
#endif

#include <assert.h>

#endif
