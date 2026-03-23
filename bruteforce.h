#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

#include "spatial.h"

// Finds all points within radius of (x, y). Results array must be pre-allocated.
void QueryBruteForce(SpatialPoint* points, int numPoints, float x, float y, float radius, int* results, int* count, int maxResults);

#endif
