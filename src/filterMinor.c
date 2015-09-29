#include "canary.h"
//#include "stopwatch.h"
#include <stdio.h>

int main(int argc, char *argv[])
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
	
	
	print_adjacency_list(&h);
	print_adjacency_list(&g);
	int haves=0, havenots=0;
	while ((getline(&gstr, &strlen, stdin)) > 1)
	{
		if (*gstr == '#') continue; // allow comments?
		printf("\"%s\"\n",gstr);
		g62setgraph(gstr, &g);
		if (has_minor(&g,&h))
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