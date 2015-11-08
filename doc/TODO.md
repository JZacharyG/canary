Todo:
=====

1. (Optionally) confirm the returned model of the minor.
2. Try out sparse mods
	- Instead of thinking about it as changing a path, and saving info for h1 and h2, think about changing branch sets, and have activehv and passivehv (or better names)
3. Try to sparsify the whole data structure
	- Get rid of c1,c2 for completed paths? Use only (semi)-assigned sets?
	- No indices, everything in terms of gv; one list in d instead of a separate one for each path
4. Experiment with different orderings of vertices of H and G.
	- There is something big going on with the ordering of G.  How should one best leverage this?  The advantage seems to be coming from the order in which we are picking the anchors.  If we can figure out why it is better, it is probably worth tailoring the ordering of our anchors to the particular context, instead of relying on a global order (which perhaps is only better on average?).
5. Refactor
	- Reorder H and G before calling the function, then use whichever order was passed in?
	- Allow you to precompute H data? (Or else don't bother having it in a different structure?)
	- Store the full graph in the data, don't recreate the structure
6. Does the order on the edges matter?
	- Is it possibly better to move onto the next branch set before finding all paths between BSs thus far considered?
7. Build a good test suite!!
8. Improve set
	- Larger maximum size
	- setget() -> sethas() ?
9. Add options and stats to filterMinor, findMinor.
10. Make arguments constant?  Makes _literally_ no difference to the comiled code, but definitely good form.
11. Build next or use a function pointer?
12. Make use of ```restrict```?
13. Keep track of components?
14. Do something with singleton BSs?
15. Create genmf?
	- to ba able to start with a specific number of vertices... take a look at edge addition as our augmentation?
		- orbital branching?
		- Think about this stuff, which method might be better...