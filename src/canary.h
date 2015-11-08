#ifndef CANARY_H
#define CANARY_H

#include <stdbool.h>
#include "setgraph.h"
typedef struct hdata hdata;


void initialize_hdata(hdata* hd, setgraph* h);

// returns true if h is a minor of g, and if hv2bs!=NULL stores a model of the minor there.
bool has_minor(setgraph* g, setgraph* h, bitset* hv2bs);

// confirms that the given model is actually a minor
bool is_minor(setgraph* g, setgraph* h, bitset* hv2bs);
#endif
