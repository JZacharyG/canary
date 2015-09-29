objects = setgraph.o canary.o
targets = findMinor filterMinor# genmf
CC = cc
CFLAGS = -g

all: $(objects) $(targets)
clean:
	rm *.o $(targets)

findMinor: findMinor.c $(objects)
	$(CC) $(CFLAGS) $< $(objects) -o $@
filterMinor: filterMinor.c $(objects)
	$(CC) $(CFLAGS) $< $(objects) -o $@
genmf: genmf.c $(objects)
	$(CC) $(CFLAGS) $< $(objects) -o $@

setgraph.o: setgraph.c setgraph.h set.h
	$(CC) -c $(CFLAGS) $<
canary.o: canary.c canary.h set.h setgraph.h debug.h
	$(CC) -c $(CFLAGS) $<
