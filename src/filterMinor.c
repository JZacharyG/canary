#include "canary.h"
//#include "stopwatch.h"
#include <stdio.h>
#include <string.h>

// [future] usage:
#define usageString "-[any|all|none] [-r[est] filename.g6] m1 [m2 ...]"
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
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s %s\n", argv[0], usageString);
		return 1;
	}
	
	int argi=1;
	filterMode mode = NONE;
	bool saveOthers = false;
	FILE* othersFile = NULL;
	
	if (!strcmp(argv[argi], "-h") || !strcmp(argv[argi], "-help") || !strcmp(argv[argi], "--help"))
	{
		fprintf(stderr, "Usage: %s %s\n", argv[0], usageString);
		return 1;
	}
	
	if (!strcmp(argv[argi], "-any"))
	{
		mode = ANY;
		++argi;
	}
	else if (!strcmp(argv[argi], "-all"))
	{
		mode = ALL;
		++argi;
	}
	else if (!strcmp(argv[argi], "-none"))
	{
		mode = NONE;
		++argi;
	}
	
	if (!strcmp(argv[argi], "-r") || !strcmp(argv[argi], "-rest"))
	{
		saveOthers = true;
		++argi;
		othersFile = fopen(argv[argi], "w");
		if (othersFile == NULL)
		{
			fprintf(stderr, "Error: Could not create file \"%s\"\n", argv[argi]);
			return 1;
		}
		++argi;
	}
	
	setgraph* minors = malloc((argc-argi)*sizeof(setgraph));
	for (int i=argi; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			fprintf(stderr, "Error: unrecognized argument %s\n", argv[i]);
			fprintf(stderr, "Usage: %s %s\n", argv[0], usageString);
			free(minors);
			return 1;
		}
		g62setgraph(argv[i], &minors[i-argi]);
	}
	int numMinors = argc-argi;
	if (numMinors == 0)
	{
		fprintf(stderr, "Error: no minors given\n");
		fprintf(stderr, "Usage: %s %s\n", argv[0], usageString);
		free(minors);
		return 1;
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
					keep=false;
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
				fprintf(othersFile, "%s", gstr);
		}
		//puts(argv[1]);
		free_setgraph(&g);
	}
	fprintf(stderr,"%d had %s of the minor(s) and %d did not.\n", kept, ((mode==ANY)?"any":(mode==ALL)?"all":"none"), rejected);
	
	for (int i=0; i< numMinors; ++i)
		free_setgraph(&minors[i]);
	free(minors);
	free(gstr);
	return 0;
}