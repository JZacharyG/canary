Todo:
=====

1. (Optionally) confirm the returned model of the minor.
2. Keep bsnbhd for past branch sets (and keep it up to date, when creating mods)
	- Modify the relevant bits of buildpath to take advantage of it.
3. Try out sparse mods
	- Instead of thinking about it as changing a path, and saving info for h1 and h2, think about changing branch sets, and have activehv and passivehv (or better names)
4. Try to sparsify the whole data structure
	- Get rid of c1,c2 for completed paths? Use only (semi)-assigned sets?
	- No indices, everything in terms of gv; one list in d instead of a separate one for each path
5. Experiment with different orderings of vertices of H and G.
	- There is something big going on with the ordering of G.  How should one best leverage this?  The advantage seems to be coming from the order in which we are picking the anchors.  If we can figure out why it is better, it is probably worth tailoring the ordering of our anchors to the particular context, instead of relying on a global order (which perhaps is only better on average?).
6. Refactor
	- Reorder H and G before calling the function, then use whichever order was passed in?
	- Allow you to precompute H data? (Or else don't bother having it in a different structure.)
	- Store the full graph in the data, don't recreate the structure
7. Does the order on the edges matter?
	- Is it possibly better to move onto the next branch set before finding all paths between BSs thus far considered?
8. Pick better anchors
	- Can we put the anchor at the beginning of the first path in the BS?  This way we can (more) quickly get a set of the nbhd of a branch set.
	- oh, shit. We can't assume that the anchor is adjacent to anything currently (semi)assigned to a neighboring branch set! Those aren't complete, maybe it is only adjacent to something that will be.
	- one thing we could do... Ensure anchor is used in an essential way:
		- if BS has at least 3 neighbors, then it has a vertex of degree at least three. Use only degree at least three gvs as anchors.
		- use first needed bs a path bumps into, instead of looking for one in particular? Harder, with current data structures. 
		- rather, if we are looking at one of the last paths, don't have three coming from the anchor... Only start paths there? Is this actually a good thing?
9. Build a good test suite!!
10. Improve set
	- Larger maximum size
	- setget() -> sethas() ?
11. Write that 'validate' function.  I feel like it might come in handy. 
12. What happens if vertices are bytes or shorts instead of ints. unsigned?
	- Convert relavent variables to type vertex instead of int, to facilitate this testing, maybe improve readability?  In any case, easy to change back later if desired.
13. Add options and stats to filterMinor, findMinor.
14. Make arguments constant?  Makes _literally_ no difference to the comiled code, but definitely good form.
15. Build next or use a function pointer?
16. Make use of ```restrict```?
17. Keep track of components?
18. Do something with singleton BSs?
19. Create genmf?
	- to ba able to start with a specific number of vertices... take a look at edge addition as our augmentation?
		- orbital branching?
		- Think about this stuff, which method might be better...