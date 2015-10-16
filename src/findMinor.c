#include "canary.h"
#include "debug.h"
//#include "stopwatch.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
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
	bitset* hv2bs = alloca(h.nv*sizeof(bitset));
	if (has_minor(&g, &h, hv2bs))
	{
		printf("Minor Found! Branch sets given by:\n");
		vertex hv, gv;
		for (hv = 0; hv < h.nv; ++hv)
		{
			printf("%d :", hv);
			if (first(hv2bs[hv], &gv)) do
			{
				printf(" %d", gv);
			} while(next(hv2bs[hv], &gv, gv));
			printf("\n");
		}
	}
	else
		printf("No such minor exists.\n");
}