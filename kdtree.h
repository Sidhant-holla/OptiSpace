#ifndef KDTREE_H
#define KDTREE_H

#include "spatial.h"

typedef struct KDNode {
    SpatialPoint data;
    struct KDNode *left, *right;
    int axis;
} KDNode;

typedef struct {
    KDNode* nodePool;
    int poolIdx;
    int maxPoints;
    KDNode* root;
} KDTree;

KDTree* InitKDTree(int maxPoints);
void RebuildKDTree_InPlace(KDTree* tree, SpatialPoint* points, int count);
void QueryKDTree(KDNode* node, float x, float y, float radius, int* results, int* count);
void FreeKDTree(KDTree* tree);

#endif
