#ifndef CANARY_H
#define CANARY_H

#include <stdbool.h>
#include "setgraph.h"
typedef struct hdata hdata;

void initialize_hdata(hdata* hd, setgraph* h);
bool has_minor(setgraph* g, setgraph* h);

#endif