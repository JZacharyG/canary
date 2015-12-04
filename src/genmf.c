#include "canary.h"
#include <stdio.h>

// generates 
// usage:
// genmf -n# [-e#] -f filenameprefix -m m1 ...
// -n specify the maximum number of vertices
// -e specify the maximum number of edges
// -f give a prefix for the created files.
// -m after this flag, list the graphs that you would like not to be minors, in g6 format

typedef enum
{
	false = 0, true = !0
} bool;

bool is_canon(bitset nbhd)
{
	return true;
}

void generate_from_graph(setgraph* g, setgraph* h, FILE* out)
{
	//for each subset of V(g)
		//if adding a vertex with this neighborhood is canonical
			//if !has_minor
				//fprintf(out)
}

void generate_from_file(FILE* in, setgraph* h, FILE* out)
{
	//for line in file
		//make the graph
}

int main(int argc, char** argv)
{
	// parse arguments
	
// 	int nv = 4;
// 	for (int n=1; n<nv; ++n)
// 	{
// 		bitset p = setmask(n);
// 		while (!setget(p,nv))
// 		{
// 			print_set(p);
// 			printf("\n");
// 			p = nextkset(p);
// 		}
// 	}
	
	setgraph g;
	gpl2setgraph(argv[1], &g);
	print_adjacency_list(&g);
}