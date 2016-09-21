#include "canary.h"
//#include "stopwatch.h"
#include <stdio.h>
#include <string.h>

// [future] usage:
#define usageString "-[any|all|none] [-split] m1 [m2 ...]"
#define helpString  "Expects any number of graphs (the potential minors, in graph6 format) to be\ngiven as arguments, and a condition (having either any, all, or none of these\nminors) on which to filter any graphs (also graph6, one per line) provided to\nstdin.  All those meeting the specified condition are written back to stdout,\nand if the -split option is given, those not meeting the condition are written\nto stderr.\n"
// Tests graphs for the graph minors given as arguments (in g6 format).
// 
// It accepts graphs in g6 format from stdin and outputs to stdout the ones that
// contain the specified portion [any, all, or none] of the graphs provided as
// arguments as minors.
// 
// Optionally, prints the graphs that fail this condition to a file given after
// the '-rest' flag.

typedef enum
{
	ANY, ALL, NONE
} filterMode;

typedef enum
{
	false = 0, true = !0
} bool;

int main(int argc, char* argv[])
{
	int argi=1;
	
	if (argc <= 1 || !strcmp(argv[argi], "-h") || !strcmp(argv[argi], "-help") || !strcmp(argv[argi], "--help"))
	{
		fprintf(stderr, "Usage: %s %s\n\n", argv[0], usageString);
		fprintf(stderr, helpString);
		return 1;
	}
	
	filterMode mode = NONE;
	bool saveOthers = false;

	while (argi < argc && argv[argi][0] == '-')
	{
		if (!strcmp(argv[argi], "-any"))
			mode = ANY;
		else if (!strcmp(argv[argi], "-all"))
			mode = ALL;
		else if (!strcmp(argv[argi], "-none"))
			mode = NONE;
		else if (!strcmp(argv[argi], "-split"))
			saveOthers = true;
		else if (!strcmp(argv[argi], "-h") || !strcmp(argv[argi], "-help") || !strcmp(argv[argi], "--help"))
		{
			fprintf(stderr, "Usage : %s %s\n\n", argv[0], usageString);
			fprintf(stderr, helpString);
			return 1;
		}
		else
		{
			fprintf(stderr, "Error : unrecognized argument %s\n", argv[argi]);
			fprintf(stderr, "Usage : %s %s\n", argv[0], usageString);
			fprintf(stderr, "Or try: %s -help\n", argv[0]);
			return 1;
		}
		++argi;
	}
	
	int numMinors = argc-argi;
	if (numMinors == 0)
	{
		fprintf(stderr, "Error : no minors given\n");
		fprintf(stderr, "Usage : %s %s\n", argv[0], usageString);
		fprintf(stderr, "Or try: %s -help\n", argv[0]);
		return 1;
	}
	setgraph* minors = malloc(numMinors*sizeof(setgraph));
	for (int i=argi; i < argc; ++i)
	{
		// will exit from inside this function if it fails to parse the graph
		g62setgraph(argv[i], &minors[i-argi]);
	}
	
	
	setgraph g;
	size_t strlen = 20; // this is automatically grown as needed via getline
	char* gstr = malloc(strlen);
	
// 	print_adjacency_list(&h);
// 	print_adjacency_list(&g);
	int kept=0, rejected=0;
	while ((getline(&gstr, &strlen, stdin)) > 1)
	{
		if (*gstr == '#') continue; // to allow comments.  I should probably add in something for a header, too.
		g62setgraph(gstr, &g);
		bool keep;
		switch (mode)
		{
		case ANY:
			keep = false;
			for (int i=0; i<numMinors; ++i)
			{
				if (has_minor(&g, &minors[i], NULL))
				{
					keep = true;
					break;
				}
			}
			break;
		case ALL:
			keep = true;
			for (int i=0; i<numMinors; ++i)
			{
				if (!has_minor(&g, &minors[i], NULL))
				{
					keep = false;
					break;
				}
			}
			break;
		case NONE:
			keep = true;
			for (int i=0; i<numMinors; ++i)
			{
				if (has_minor(&g, &minors[i], NULL))
				{
					keep = false;
					break;
				}
			}
			break;
		}
		
		if (keep)
		{
			++kept;
			printf("%s",gstr);
		}
		else
		{
			++rejected;
			if (saveOthers)
				fprintf(stderr, "%s", gstr);
		}
		free_setgraph(&g);
	}
	if (!saveOthers)
		fprintf(stderr,"%d had %s of the minor(s) and %d did not.\n", kept, ((mode==ANY)?"any":(mode==ALL)?"all":"none"), rejected);
	
	for (int i=0; i < numMinors; ++i)
		free_setgraph(&minors[i]);
	free(minors);
	free(gstr);
	return 0;
}