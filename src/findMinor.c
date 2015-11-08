#include "canary.h"
#include "debug.h"
//#include "stopwatch.h"
#include <stdio.h>
#include <string.h>

#define usageString "[-p] minor graph"
#define usageExit { fprintf(stderr, "Usage: %s %s\n", argv[0], usageString); return 1; }
int main(int argc, char *argv[])
{
	if (argc < 3) usageExit;
	
	
	
	
	setgraph h, g;
	if (!strcmp(argv[1], "-p"))
	{
		if (argc < 4) usageExit;
		gpl2setgraph(argv[2], &h);
		gpl2setgraph(argv[3], &g);
	}
	else
	{
		g62setgraph(argv[1], &h);
		g62setgraph(argv[2], &g);
	}
	
	if (DEBUG)
	{
		print_adjacency_list(&h);
		print_adjacency_list(&g);
	}
	bitset* hv2bs = alloca(h.nv*sizeof(bitset));
	if (has_minor(&g, &h, hv2bs))
	{
		printf("Minor Found! Branch sets given by:\n");
		vertex hv, gv;
		for (hv = 0; hv < h.nv; ++hv)
		{
			printf("%d : ", hv);
			print_set(hv2bs[hv]);
			printf("\n");
		}
	}
	else
		printf("No such minor exists.\n");
	
	free_setgraph(&g);
	free_setgraph(&h);
}