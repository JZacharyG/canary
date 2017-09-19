# Canary
*Tests things for minors.*

Canary is a general purpose and relatively speedy graph minor tester for simple graphs.  Specifically, given two simple graphs G and H, it will tell you if H is a minor of G and provide you with a model of H in G.  For your convenience, Canary comes bundled with a few simple command-line utilities, including `findMinor` for testing a single graph for a single minor (both taken as command line arguments), and `filterMinor` for filtering a list of graphs for those with any, all, or none of a list of minors.

## Requirements and Installation
Go download the latest release, navigate to the project's folder on the command line, and run `make`.  Just like that, you should be up and running! (If not, please send me an email.  This is definitely something I would like to know about.)  All executables will be stored in the directory `exe/`.

Canary comes bundled with nauty, a program written by Brendan McKay to compute the automorphism group of a graph.  It is used to compute automorphisms of the proposed minor, which can speed the search up quite considerably if that graph has lots of symmetries.  If something goes wrong, or you would not *like* to use nauty, simply run `make nonauty` instead.

## Usage

Let's say you can't remember how to get K\_3,3 as a minor of the Petersen graph.  Don't worry, we've all been there.
As it turns out, `EFz_` and `IheA@GUAo` are, respectively, [graph6](http://users.cecs.anu.edu.au/~bdm/data/formats.txt) encodings of these graphs.

``` 
findMinor 'EFz_' 'IheA@GUAo' 
```
The output will contain something like the following, giving a branch set for each vertex in K\_3,3, (i.e. a connected subset of vertices in the Petersen graph such that two branch sets are adjacent whenever their corresponding vertices are adjacent in K\_3,3).

```
Minor Found! Branch sets given by:
0 : 5
1 : 4 9
2 : 1 6
3 : 3 8
4 : 2 7
5 : 0
```

Maybe you don't really like this, because you don't know the graph6 code for every graph you care about.  Worry not.  You can instead describe your graph as a space delimited list of walks which together cover every edge!  Just give the `-p` flag and you are on your way.

```
findMinor -p 'adbecfae bf cd.' 'abcdea fhjgif af bg ch di ej.'
```
There are other things that you can do, but that should be enough to get you started.

##Tests
I've done a hodgepodge of testing, but the test suite is still quite limited.  You can run whatever tests I have with `make tests` or by running the program `runTests`.  If you have an idea for a family of graphs that I should test it on, or if, like me, you have a few sets of graphs lying around that you know either have or do not have a certain minor, then please let me know.  It would be greatly appreciated!

## How you can help

This project is still very young, and the very best thing that you can do is to use it.  Use it, test it, love it, and tell me how it goes.  If it is helpful, tell your friends!

I would love to hear what sort of projects you are using Canary for, and please feel free to tell me about any bugs you find, or suggestions you have.
