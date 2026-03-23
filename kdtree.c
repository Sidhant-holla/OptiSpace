#include "kdtree.h"
#include<stdlib.h>
#include<math.h>

void QuickSelect(SpatialPoint* points, int left, int right, int k, int axis){
    while (left < right) {
        int mid = left + (right - left) / 2;
        float a = (axis == 0) ? points[left].x : points[left].y;
        float b = (axis == 0) ? points[mid].x : points[mid].y;
        float c = (axis == 0) ? points[right].x : points[right].y;
        // Median of three: put median-valued point at points[left]
        if ((b >= a && b <= c) || (b <= a && b >= c)) {
            SpatialPoint temp = points[left];
            points[left] = points[mid];
            points[mid] = temp;
        }
        else if ((c >= a && c <= b) || (c <= a && c >= b)) {
            SpatialPoint temp = points[left];
            points[left] = points[right];
            points[right] = temp;
        }
        float pivotVal = (axis == 0) ? points[left].x : points[left].y;
        int i = left - 1;
        int j = right + 1;
        if (axis == 0) {
            while (1) {
                do { i++; } while (points[i].x < pivotVal);
                do { j--; } while (points[j].x > pivotVal);
                if (i >= j) break;
                SpatialPoint temp = points[i];
                points[i] = points[j];
                points[j] = temp;
            }
        } else {
            while (1) {
                do { i++; } while (points[i].y < pivotVal);
                do { j--; } while (points[j].y > pivotVal);
                if (i >= j) break;
                SpatialPoint temp = points[i];
                points[i] = points[j];
                points[j] = temp;
            }
        }
        if (k <= j) right = j;
        else left = j + 1;
    }
}


KDTree* InitKDTree(int maxPoints){
    KDTree* tree = (KDTree*)malloc(sizeof(KDTree));
    tree->maxPoints = maxPoints;
    tree->nodePool = (KDNode*)malloc(sizeof(KDNode)*maxPoints);
    tree->poolIdx = 0;
    tree->root = NULL;
    return tree;
}

KDNode* BuildRec(KDTree* tree, SpatialPoint* points, int n, int depth){
    if(n<=0) return NULL;
    int axis = depth%2;
    QuickSelect(points, 0, n-1, n/2, axis);
    int mid = n/2;
    KDNode* node = &tree->nodePool[tree->poolIdx++];
    node->data = points[mid];
    node->axis = axis;
    node->left = BuildRec(tree, points, mid, depth+1);
    node->right = BuildRec(tree, points+mid+1, n-mid-1, depth+1);
    return node; 
}

void RebuildKDTree_InPlace(KDTree *tree, SpatialPoint *points, int n ){
    tree->poolIdx = 0;
    tree->root = BuildRec(tree, points, n, 0);
}

void QueryKDTree(KDNode *node, float x, float y, float radius, int* results, int* count, int maxResults){
    if (node == NULL) return;
    float xdist = node->data.x - x;
    float ydist = node->data.y - y;
    float distSquare = xdist*xdist + ydist*ydist;
    if (distSquare < radius*radius && distSquare > 0.001f){
        if (*count < maxResults) {
            results[*count] = node->data.index;
            (*count)++;
        }
    }
    float diff = 0;
    if (node->axis == 0) diff = x - node->data.x;
    else diff = y - node->data.y;
    if (diff <= 0){
        QueryKDTree(node->left, x, y, radius, results, count, maxResults);
            if (fabs(diff) < radius) QueryKDTree(node->right, x, y, radius, results, count, maxResults);
    }
    else if (diff > 0){
        QueryKDTree(node->right, x, y, radius, results, count, maxResults);
            if (fabs(diff) < radius) QueryKDTree(node->left, x, y, radius, results, count, maxResults);
    }

}

void FreeKDTree(KDTree *tree) {
    free(tree->nodePool);
    free(tree);
}
