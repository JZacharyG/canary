# Canary
*Tests things for minors.*

Canary is a general purpose and relatively speedy graph minor tester for simple graphs.  Specifically, given two simple graphs G and H, it will tell you if H is a minor of G and provide you with a model of H in G.  For your convenience, Canary comes bundled with a few simple command-line utilities, including ```findMinor``` for testing a single graph for a single minor (both taken as command line arguments), and ```filterMinor``` for filtering a list of graphs for those with any, all, or none of a list of minors.

## Requirements
Canary would like very much to use nauty, a program written by Brendan McKay to compute the automorphism group of a graph, but it is not required.  It is used to compute automorphisms of H, which can make a huge difference when testing for highly symmetric minors.

If you would like to take advantage of this functionality, you may need to adjust the makefile so that the variable ```NAUTY``` points to the correct directory.  If not, you should compile with ```make nonauty```.