#include "bruteforce.h"

void QueryBruteForce(SpatialPoint* points, int numPoints, float x, float y, float radius, int* results, int* count, int maxResults) {
    float radiusSq = radius * radius;
    *count = 0;
    for (int i = 0; i < numPoints; i++) {
        float dx = points[i].x - x;
        float dy = points[i].y - y;
        float distSq = dx * dx + dy * dy;
        if (distSq < radiusSq && distSq > 0.001f) {
            if (*count >= maxResults) break;
            results[(*count)++] = points[i].index;
        }
    }
}
