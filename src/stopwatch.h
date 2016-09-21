#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <time.h>

typedef struct
{
	clock_t start, total;
} stopwatch;

#define CYCPERSEC 1000000UL // Seems about right; I make no guarantees.

#define reset_stopwatch(s) ((s).total=0)
#define start_stopwatch(s) ((s).start=clock())
#define pause_stopwatch(s) ((s).total+=clock()-(s).start)
#define read_stopwatch(s) ((s).total)
#define fprint_stopwatch(f,s) \
{ \
	if ((s).total>=24*3600*CYCPERSEC) \
		fprintf((f),"%luh ", (s).total/(24*3600*CYCPERSEC)); \
	if ((s).total>=3600*CYCPERSEC) \
		fprintf((f),"%luh ", ((s).total%(24*3600*CYCPERSEC)) / (3600*CYCPERSEC)); \
	if ((s).total>=60*CYCPERSEC) \
		fprintf((f),"%lum ", ((s).total%(3600*CYCPERSEC)) / (60*CYCPERSEC)); \
	if ((s).total>=CYCPERSEC) \
		fprintf((f),"%lu", ((s).total%(60*CYCPERSEC)) / CYCPERSEC); \
	else \
		fprintf((f), "0"); \
	fprintf((f),".%03lus", ((s).total%CYCPERSEC)*1000 / CYCPERSEC); \
}
#define print_stopwatch(s) fprint_stopwatch(stdout,(s))


#endif