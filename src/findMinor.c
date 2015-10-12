#include "canary.h"
#include "debug.h"
//#include "stopwatch.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("%s %s\n", argv[1], argv[2]);
	if (argc != 3)
	{
		fprintf(stderr, "I'm expecting two graph6 codes and you aren't delivering.\n");
		return 1;
	}
	
	
	setgraph h, g;
	g62setgraph(argv[1], &h);
	g62setgraph(argv[2], &g);
	
	if (DEBUG)
	{
		print_adjacency_list(&h);
		print_adjacency_list(&g);
	}
	
	if (has_minor(&g,&h))
		printf("Has the Minor!\n");
	else
		printf("Doesn't have the minor...\n");
}