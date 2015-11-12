# Canary
*Tests things for minors.*

Canary is a general purpose and relatively speedy graph minor tester for simple graphs.  Specifically, given two simple graphs G and H, it will tell you if H is a minor of G and provide you with a model of H in G.  For your convenience, Canary comes bundled with a few simple command-line utilities, including `findMinor` for testing a single graph for a single minor (both taken as command line arguments), and `filterMinor` for filtering a list of graphs for those with any, all, or none of a list of minors.

## Requirements and Installation
Canary would like very much to use nauty, a program written by Brendan McKay to compute the automorphism group of a graph, but it is not required.  It is used to compute automorphisms of H, which can make a huge difference when testing for highly symmetric minors.

In any case, go download or clone a copy and navigate to the project's folder on the command line. If you would like to use Canary to use nauty, you will need to edit the makefile so that the variable `NAUTY` points to the correct directory, at which point you should run `make`.  Otherwise, or if you are just super excited and want to get going as soon as is possible, you should simply compile with `make nonauty`.

And just like that, you should be up and running!
(If not, please send me an email.  This is definitely something I would like to know about.)

A word of caution: I have only tested this on a 64-bit machine running OS X, compiled with Clang.  Canary currently uses some builtin functions that are defined in gcc and Clang, but it should switch to reasonable alternatives if you use a different compiler.

##Usage
Let's say you can't remember how to get K\_3,3 as a minor of the Petersen graph.  Don't worry, we've all been there.
As it turns out, `EFz_` and `IheA@GUAo` are, respectively, graph6 encodings of these graphs.
Just run the following command, and you will be informed of which vertices of the Petersen graph are in the branch set of each vertex of K\_3,3 (i.e. which vertices should be contracted together to form the vertices in the minor).
``` 
findMinor 'EFz_' 'IheA@GUAo'
```
Maybe you don't really like this, because you don't know the graph6 code for every graph you care about.  Worry not.  You can instead describe your graph by listing paths that together cover every edge!
```
findMinor -p 'adbecfae bf cd.' 'abcdea fhjgif af bg ch di ej.'
```
There are other things that you can do, but that should be enough to get you started.

##Tests
I've done a hodgepodge of testing, but honestly don't have an nice organized suite yet.  If you have any ideas for a family of graphs that I can test it on, or if, like me, you have a few sets of graphs that you know have or do not have a certain minor, then please let me know.  It would be greatly appreciated.

##How you can help
This project is still very young, and the very best thing that you can do is to use it.  Use it, test it, love it, and tell me how it goes.  If it is helpful, tell your friends!

I would love to hear what sort of projects you are using Canary for, and please feel free to tell me about any bugs you find, or suggestions you have.

##License
There will probably be a license at some point.
