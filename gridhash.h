#ifndef GRIDHASH_H
#define GRIDHASH_H

#include "spatial.h"

typedef struct {
    int* cellStart;       // prefix-sum boundaries, size = numCells + 1
    SpatialPoint* sorted; // agents sorted by cell
    int* tempCounts;      // scratch for counting sort
    int cols, rows;
    int numCells;
    float cellSize;
    float originX, originY;
} SpatialGrid;

SpatialGrid* InitGrid(float originX, float originY, float width, float height, float cellSize, int maxPoints);
void BuildGrid(SpatialGrid* grid, SpatialPoint* points, int count);
void QueryGrid(SpatialGrid* grid, float x, float y, float radius, int* results, int* count, int maxResults);
void FreeGrid(SpatialGrid* grid);

#endif
