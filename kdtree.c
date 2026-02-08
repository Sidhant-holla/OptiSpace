#include "kdtree.h"
#include<string.h>
#include<stdlib.h>
#include<math.h>

int CmpX(const void*a, const void*b){
    KDPoint* p1 = (KDPoint*)a;
    KDPoint* p2 = (KDPoint*)b;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

int CmpY(const void*a, const void*b){
    KDPoint* p1 = (KDPoint*)a;
    KDPoint* p2 = (KDPoint*)b;
    return (p1->y > p2->y) - (p1->y < p2->y);
}

KDTree* InitKDTree(int maxPoints){
    KDTree* tree = (KDTree*)malloc(sizeof(KDTree));
    tree->maxPoints = maxPoints;
    tree->nodePool = (KDNode*)malloc(sizeof(KDNode)*maxPoints);
    tree->poolIdx = 0;
    tree->root = NULL;
    return tree;
}

KDNode* BuildRec(KDTree* tree, KDPoint* points, int n, int depth){
    if(n<=0) return NULL;
    int axis = depth%2;
    if(axis == 0){
        qsort(points, n, sizeof(KDPoint),CmpX);
    }
    else{
        qsort(points, n, sizeof(KDPoint), CmpY);
    }
    int mid = n/2;
    KDNode* node = &tree->nodePool[tree->poolIdx++];
    node->data = points[mid];
    node->axis = axis;
    node->left = BuildRec(tree, points, mid, depth+1);
    node->right = BuildRec(tree, points+mid+1, n-mid-1, depth+1);
    return node;
}
