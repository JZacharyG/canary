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
int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "I'm expecting a graph6 for the minor and you aren't delivering.\n");
		return 1;
	}
	setgraph h, g;
	size_t strlen = 20;
	char* gstr = malloc(strlen);
	g62setgraph(argv[1], &h);
	
	
// 	print_adjacency_list(&h);
// 	print_adjacency_list(&g);
	int haves=0, havenots=0;
	while ((getline(&gstr, &strlen, stdin)) > 1)
	{
		if (*gstr == '#') continue; // allow comments?
		//printf("\"%s\"\n",gstr);
		g62setgraph(gstr, &g);
		if (has_minor(&g, &h, NULL))
		{
			++haves;
		}
		else
		{
			++havenots;
		}
		//puts(argv[1]);
	}
	printf("\n%d had the minor and %d did not\n", haves, havenots);
}