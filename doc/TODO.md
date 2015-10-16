Todo:
=====

1. Return a model of the minor.
2. Symmetry
	- Don't use G-vertices that don't leave room for the symmetric vertices.
3. Experiment with different orderings of vertices of H and G.
	- There is something big going on with the ordering of G.  How should one best leverage this?
4. Pick better anchors
	- Can we put the anchor at the beginning of the first path in the BS?  This way we can (more) quickly get a set of the nbhd of a branch set.
	- oh, shit. We can't assume that the anchor is adjacent to anything currently (semi)assigned to a neighboring branch set! Those aren't complete, maybe it is only adjacent to something that will be.
	- one thing we could do... Ensure anchor is used in an essential way:
		- if BS has at least 3 neighbors, then it has a vertex of degree at least three. Use only degree at least three gvs as anchors.
		- use first needed bs a path bumps into, instead of looking for one in particular? Harder, with current data structures. 
		- rather, if we are looking at one of the last paths, don't have three coming from the anchor... Only start paths there? Is this actually a good thing?
5. Try out sparse mods
6. Try to sparsify the whole data structure
	- Get rid of c1,c2 for completed paths? Use only (semi)-assigned sets?
	- No indices, everything in terms of gv; one list in d instead of a separate one for each path
7. Build a good test suite
8. Improve set?
	- Larger maximum size
	- setget() -> sethas()
9. Write that 'validate' function.  I feel like it might come in handy. 
10. What happens if vertices are bytes or shorts instead of ints. unsigned?
	- Convert relavent variables to type vertex instead of int, to facilitate this testing, maybe improve readability?  In any case, easy to change back later if desired.
11. Make filterMinor do what it needs to.
13. Make arguments constant?  Makes _literally_ no difference to the comiled code.
14. Build next, or just use a function pointer?
15. Make use of ```restrict```?
16. Keep track of components?
17. Create genmf