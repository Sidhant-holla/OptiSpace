#include "agent.h"
#include "config.h"
#include "spatial.h"
#include "bruteforce.h"
#include "kdtree.h"
#include <math.h>
#include <stdlib.h>

static KDTree* tree = NULL;
static SpatialPoint* spatialBuffer = NULL;

void InitAgents(Agent agents[], int count) {
    spatialBuffer = (SpatialPoint*)malloc(sizeof(SpatialPoint) * count);
    for (int i = 0; i < count; i++) {
        agents[i].x = Radius + rand() % (ScreenWidth - 2 * Radius);
        agents[i].y = UIHeight + Radius + rand() % (ScreenHeight - UIHeight - 2 * Radius);
        agents[i].vx = 0.0f;
        agents[i].vy = 0.0f;
        agents[i].speed = (50 + rand() % ((int)(MaxSpeed * 100) - 50)) / 100.0f;
        agents[i].sepVx = 0.0f;
        agents[i].sepVy = 0.0f;
        agents[i].neighbourCount = 0;
    }
}

void ComputePerception(Agent Point[], int count, int useKDTree) {
    for (int i = 0; i < count; i++) {
        spatialBuffer[i].x = Point[i].x;
        spatialBuffer[i].y = Point[i].y;
        spatialBuffer[i].index = i;
    }

    if (useKDTree) {
        if (!tree) tree = InitKDTree(count);
        RebuildKDTree_InPlace(tree, spatialBuffer, count);
    }

    for (int i = 0; i < count; i++) {
        int neighbourCount = 0;
        Point[i].sepVx = 0;
        Point[i].sepVy = 0;

        int results[MaxNeighbours];

        if (useKDTree) {
            QueryKDTree(tree->root, Point[i].x, Point[i].y, NeighbourRadius, results, &neighbourCount, MaxNeighbours);
        } else {
            QueryBruteForce(spatialBuffer, count, Point[i].x, Point[i].y, NeighbourRadius, results, &neighbourCount, MaxNeighbours);
        }

        for (int j = 0; j < neighbourCount; j++) {
            int ni = results[j];
            float dx = Point[i].x - Point[ni].x;
            float dy = Point[i].y - Point[ni].y;
            float distSq = dx * dx + dy * dy;
            Point[i].sepVx += dx / distSq;
            Point[i].sepVy += dy / distSq;
        }

        Point[i].neighbourCount = neighbourCount;
    }
}

void UpdatePhysics(Agent Point[], int count, float targetX, float targetY, float dt) {
    for (int i = 0; i < count; i++) {
        float dx = Point[i].x - targetX;
        float dy = Point[i].y - targetY;
        float length = sqrtf(dx * dx + dy * dy);

        float fleeVx = 0;
        float fleeVy = 0;

        if (length > 0.001f && length < 500.0f) {
            fleeVx = (dx / length) * Point[i].speed;
            fleeVy = (dy / length) * Point[i].speed;
        }

        float separationVx = Point[i].sepVx * SepMultiplier;
        float separationVy = Point[i].sepVy * SepMultiplier;

        float desiredVx = fleeVx + separationVx;
        float desiredVy = fleeVy + separationVy;

        float ax = (desiredVx - Point[i].vx) * SteeringForce;
        float ay = (desiredVy - Point[i].vy) * SteeringForce;

        float aMag = sqrtf(ax * ax + ay * ay);
        if (aMag > MaxForce) {
            ax = (ax / aMag) * MaxForce;
            ay = (ay / aMag) * MaxForce;
        }

        Point[i].vx += dt * ax;
        Point[i].vy += dt * ay;

        float vMag = sqrtf(Point[i].vx * Point[i].vx + Point[i].vy * Point[i].vy);
        if (vMag > MaxSpeed) {
            Point[i].vx = (Point[i].vx / vMag) * MaxSpeed;
            Point[i].vy = (Point[i].vy / vMag) * MaxSpeed;
        }

        Point[i].x += dt * Point[i].vx;
        Point[i].y += dt * Point[i].vy;

        if (Point[i].x > ScreenWidth - Radius) { Point[i].x = ScreenWidth - Radius; Point[i].vx *= -1; }
        if (Point[i].x < Radius) { Point[i].x = Radius; Point[i].vx *= -1; }
        if (Point[i].y > ScreenHeight - Radius) { Point[i].y = ScreenHeight - Radius; Point[i].vy *= -1; }
        if (Point[i].y < UIHeight + Radius) { Point[i].y = UIHeight + Radius; Point[i].vy *= -1; }
    }
}

void CleanupAgents(void) {
    if (tree) { FreeKDTree(tree); tree = NULL; }
    if (spatialBuffer) { free(spatialBuffer); spatialBuffer = NULL; }
}
