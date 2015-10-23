#include "canary.h"
//#include "stopwatch.h"
#include <stdio.h>

// [future] usage:
//   filterMinor -[any|all|none] [-r[est] filename.g6] [-s[tats]] [-q[uiet]] -m m1 ...
// Tests graphs for the graph minors given as arguments (in g6 format).
// 
// It accepts graphs in g6 format from stdin and outputs to stdout the ones that
// contain the specified portion [any, all, or none] of the graphs provided as
// arguments as minors.
// 
// Optionally, prints the graphs that fail this condition to a file given after
// the '-rest' flag.
// If the flag '-stats' is given, this program will print to stderr the number of input graphs that were filtered out and the number that were kept, as well as the time spent checking for minors.
// If the flag '-quiet' is given, then only the statistics will be printed, to stdout.

typedef enum
{
	ANY, ALL, NONE
} filterMode;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "I'm expecting a graph6 for the minor and you aren't delivering.\n");
		return 1;
	}
	
	setgraph* minors = malloc((argc-1)*sizeof(setgraph));
	
	for (int i=1; i<argc; ++i)
	{
		g62setgraph(argv[i], &minors[i-1]);
	}
	int numMinors = argc-1; // FIX ME!
	setgraph g;
	size_t strlen = 20;
	char* gstr = malloc(strlen);
	
	filterMode mode = NONE;
// 	print_adjacency_list(&h);
// 	print_adjacency_list(&g);
	int kept=0, rejected=0;
	bool hasAll, hasNone;
	while ((getline(&gstr, &strlen, stdin)) > 1)
	{
		if (*gstr == '#') continue; // allow comments?
		//printf("\"%s\"\n",gstr);
		g62setgraph(gstr, &g);
		switch (mode)
		{
		case ANY:
			++rejected;
			for (int i=0; i<numMinors; ++i)
			{
				if (has_minor(&g, &minors[i], NULL))
				{
					--rejected; ++kept;
					printf("%s",gstr);
					break;
				}
			}
			break;
		case ALL:
			hasAll = true;
			for (int i=0; i<numMinors; ++i)
			{
				if (!has_minor(&g, &minors[i], NULL))
				{
					hasAll=false;
					break;
				}
			}
			if (hasAll)
			{
				++kept;
				printf("%s",gstr);
			}
			else
				++rejected;
			break;
		case NONE:
			hasNone = true;
			for (int i=0; i<numMinors; ++i)
			{
				if (has_minor(&g, &minors[i], NULL))
				{
					hasNone=false;
					break;
				}
			}
			if (hasNone)
			{
				++kept;
				printf("%s",gstr);
			}
			else
				++rejected;
			break;
		}
		//puts(argv[1]);
	}
	fprintf(stderr,"%d had %s of the minor(s) and %d did not.\n", kept, ((mode==ANY)?"any":(mode==ALL)?"all":"none"), rejected);
}