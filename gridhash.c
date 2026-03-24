#include "gridhash.h"
#include <stdlib.h>
#include <string.h>

SpatialGrid* InitGrid(float originX, float originY, float width, float height, float cellSize, int maxPoints) {
    SpatialGrid* grid = (SpatialGrid*)malloc(sizeof(SpatialGrid));
    grid->cellSize = cellSize;
    grid->originX = originX;
    grid->originY = originY;
    grid->cols = (int)(width / cellSize) + 2;
    grid->rows = (int)(height / cellSize) + 2;
    grid->numCells = grid->cols * grid->rows;
    grid->cellStart = (int*)malloc((grid->numCells + 1) * sizeof(int));
    grid->sorted = (SpatialPoint*)malloc(maxPoints * sizeof(SpatialPoint));
    grid->tempCounts = (int*)malloc(grid->numCells * sizeof(int));
    return grid;
}

static inline int cellIdx(SpatialGrid* grid, float x, float y) {
    int cx = (int)((x - grid->originX) / grid->cellSize);
    int cy = (int)((y - grid->originY) / grid->cellSize);
    if (cx < 0) cx = 0;
    if (cx >= grid->cols) cx = grid->cols - 1;
    if (cy < 0) cy = 0;
    if (cy >= grid->rows) cy = grid->rows - 1;
    return cy * grid->cols + cx;
}

void BuildGrid(SpatialGrid* grid, SpatialPoint* points, int count) {
    memset(grid->cellStart, 0, (grid->numCells + 1) * sizeof(int));

    // Count per cell (offset by 1 for prefix sum)
    for (int i = 0; i < count; i++) {
        int cell = cellIdx(grid, points[i].x, points[i].y);
        grid->cellStart[cell + 1]++;
    }

    // Prefix sum
    for (int i = 1; i <= grid->numCells; i++)
        grid->cellStart[i] += grid->cellStart[i - 1];

    // Copy starts as write cursors
    memcpy(grid->tempCounts, grid->cellStart, grid->numCells * sizeof(int));

    // Scatter into sorted array
    for (int i = 0; i < count; i++) {
        int cell = cellIdx(grid, points[i].x, points[i].y);
        grid->sorted[grid->tempCounts[cell]++] = points[i];
    }
}

void QueryGrid(SpatialGrid* grid, float x, float y, float radius, int* results, int* count, int maxResults) {
    int cx = (int)((x - grid->originX) / grid->cellSize);
    int cy = (int)((y - grid->originY) / grid->cellSize);
    float r2 = radius * radius;
    *count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int nx = cx + dx, ny = cy + dy;
            if (nx < 0 || nx >= grid->cols || ny < 0 || ny >= grid->rows) continue;
            int cell = ny * grid->cols + nx;
            for (int i = grid->cellStart[cell]; i < grid->cellStart[cell + 1]; i++) {
                float px = grid->sorted[i].x - x;
                float py = grid->sorted[i].y - y;
                float d2 = px * px + py * py;
                if (d2 < r2 && d2 > 0.001f) {
                    if (*count >= maxResults) return;
                    results[(*count)++] = grid->sorted[i].index;
                }
            }
        }
    }
}

void FreeGrid(SpatialGrid* grid) {
    free(grid->cellStart);
    free(grid->sorted);
    free(grid->tempCounts);
    free(grid);
}
