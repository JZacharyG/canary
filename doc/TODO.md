Todo:
=====
**Version 1.1:**

1. Return a model of the minor.
2. Symmetry
3. Pick a better ordering of vertices, mostly H, also G.
4. Pick better anchors
	- Can we put the anchor at the beginning of the first path in the BS?  This way we can (more) quickly get a set of the nbhd of a branch set.
5. Try out sparse mods
6. Try to sparsify the whole data structure
	- Get rid of c1,c2 for completed paths? Use only (semi)-assigned sets?
	- No indices, everything in terms of gv; one list in d instead of a separate one for each path
7. Build a good test suite
8. Improve set?
	- Larger maximum size
	- setget() -> sethas()
	- issubset()
9. Write that 'validate' function.  I feel like it might come in handy. 
10. What happens if vertices are bytes or shorts instead of ints. unsigned?
	- Convert relavent variables to type vertex instead of int, to facilitate this testing, maybe improve readability?  In any case, easy to change back later if desired.
11. Make filterMinor do what it needs to.
12. Organize the files better, folders?
13. Make arguments constant?  Makes _literally_ no difference to the comiled code.
14. Build next, or just use a function pointer?
15. Make use of ```restrict```?

**Version 2.0**

1. Keep track of components?
2. Create genmf