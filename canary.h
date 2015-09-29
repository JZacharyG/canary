#ifndef CANARY_H
#define CANARY_H

#include <stdbool.h>
#include "setgraph.h"
typedef struct hdata hdata;

void initialize_hdata(hdata* hd, const setgraph* const h);
bool has_minor(const setgraph* const g, const setgraph* const h);

#endif