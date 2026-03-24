// Agent behavior uses Reynolds flocking model with three forces:
//   Flee       - repel from fire source
//   Separation - repel from nearby neighbors (inverse-square)
//   Alignment  - steer toward average neighbor velocity
//   Exit seek  - steer toward exit zone
// Forces are combined into a desired velocity, steered toward, clamped, then Euler-integrated.

#include "agent.h"
#include "config.h"
#include "spatial.h"
#include "bruteforce.h"
#include "kdtree.h"
#include <math.h>
#include <stdlib.h>

static KDTree* tree = NULL;
static SpatialPoint* spatialBuffer = NULL;
static int allocatedCapacity = 0;

static void EnsureCapacity(int count) {
    if (count <= allocatedCapacity) return;
    allocatedCapacity = count + count / 2; // grow by 1.5x
    spatialBuffer = (SpatialPoint*)realloc(spatialBuffer, sizeof(SpatialPoint) * allocatedCapacity);
    if (tree) { FreeKDTree(tree); tree = NULL; }
    tree = InitKDTree(allocatedCapacity);
}

void InitAgents(Agent agents[], int count) {
    EnsureCapacity(count);
    float minX = ROOM_X + WALL_THICK + Radius;
    float maxX = ROOM_X + ROOM_W - WALL_THICK - Radius;
    float minY = ROOM_Y + WALL_THICK + Radius;
    float maxY = ROOM_Y + ROOM_H - WALL_THICK - Radius;
    for (int i = 0; i < count; i++) {
        agents[i].x = minX + rand() % (int)(maxX - minX);
        agents[i].y = minY + rand() % (int)(maxY - minY);
        agents[i].vx = 0.0f;
        agents[i].vy = 0.0f;
        agents[i].speed = (500 + rand() % ((int)(MaxSpeed * 100) - 500)) / 100.0f;
        agents[i].sepVx = 0.0f;
        agents[i].sepVy = 0.0f;
        agents[i].alignVx = 0.0f;
        agents[i].alignVy = 0.0f;
        agents[i].neighbourCount = 0;
        agents[i].active = 1;
    }
}

void ComputePerception(Agent agents[], int count, int spatialMode) {
    EnsureCapacity(count);

    int activeCount = 0;
    for (int i = 0; i < count; i++) {
        if (!agents[i].active) continue;
        spatialBuffer[activeCount].x = agents[i].x;
        spatialBuffer[activeCount].y = agents[i].y;
        spatialBuffer[activeCount].index = i;
        activeCount++;
    }

    if (spatialMode == MODE_KDTREE) {
        RebuildKDTree_InPlace(tree, spatialBuffer, activeCount);
    }

    #pragma omp parallel for schedule(dynamic, 64)
    for (int i = 0; i < count; i++) {
        if (!agents[i].active) continue;
        int neighbourCount = 0;
        agents[i].sepVx = 0;
        agents[i].sepVy = 0;
        agents[i].alignVx = 0;
        agents[i].alignVy = 0;

        int results[MaxNeighbours];

        if (spatialMode == MODE_KDTREE) {
            QueryKDTree(tree->root, agents[i].x, agents[i].y, NeighbourRadius, results, &neighbourCount, MaxNeighbours);
        } else {
            QueryBruteForce(spatialBuffer, activeCount, agents[i].x, agents[i].y, NeighbourRadius, results, &neighbourCount, MaxNeighbours);
        }

        for (int j = 0; j < neighbourCount; j++) {
            int ni = results[j];
            float dx = agents[i].x - agents[ni].x;
            float dy = agents[i].y - agents[ni].y;
            float distSq = dx * dx + dy * dy;
            agents[i].sepVx += dx / distSq;
            agents[i].sepVy += dy / distSq;
            agents[i].alignVx += agents[ni].vx;
            agents[i].alignVy += agents[ni].vy;
        }
        if (neighbourCount > 0) {
            agents[i].alignVx /= neighbourCount;
            agents[i].alignVy /= neighbourCount;
        }

        agents[i].neighbourCount = neighbourCount;
    }
}

void UpdatePhysics(Agent agents[], int count, float fireX, float fireY, float exitX, float exitY, float dt) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < count; i++) {
        if (!agents[i].active) continue;

        // Flee from fire
        float dx = agents[i].x - fireX;
        float dy = agents[i].y - fireY;
        float length = sqrtf(dx * dx + dy * dy);

        float fleeVx = 0;
        float fleeVy = 0;

        if (length > 0.001f && length < FleeRadius) {
            fleeVx = (dx / length) * agents[i].speed;
            fleeVy = (dy / length) * agents[i].speed;
        }

        // Seek exit
        float edx = exitX - agents[i].x;
        float edy = exitY - agents[i].y;
        float elen = sqrtf(edx * edx + edy * edy);
        float seekVx = 0, seekVy = 0;
        if (elen > 0.001f) {
            seekVx = (edx / elen) * ExitAttraction;
            seekVy = (edy / elen) * ExitAttraction;
        }

        float separationVx = agents[i].sepVx * SepMultiplier;
        float separationVy = agents[i].sepVy * SepMultiplier;
        float alignVx = agents[i].alignVx * AlignMultiplier;
        float alignVy = agents[i].alignVy * AlignMultiplier;

        float desiredVx = fleeVx + separationVx + alignVx + seekVx;
        float desiredVy = fleeVy + separationVy + alignVy + seekVy;

        float ax = (desiredVx - agents[i].vx) * SteeringForce;
        float ay = (desiredVy - agents[i].vy) * SteeringForce;

        float aMag = sqrtf(ax * ax + ay * ay);
        if (aMag > MaxForce) {
            ax = (ax / aMag) * MaxForce;
            ay = (ay / aMag) * MaxForce;
        }

        agents[i].vx += dt * ax;
        agents[i].vy += dt * ay;

        float vMag = sqrtf(agents[i].vx * agents[i].vx + agents[i].vy * agents[i].vy);
        if (vMag > MaxSpeed) {
            agents[i].vx = (agents[i].vx / vMag) * MaxSpeed;
            agents[i].vy = (agents[i].vy / vMag) * MaxSpeed;
        }

        agents[i].x += dt * agents[i].vx;
        agents[i].y += dt * agents[i].vy;
    }
}

void CleanupAgents(void) {
    if (tree) { FreeKDTree(tree); tree = NULL; }
    if (spatialBuffer) { free(spatialBuffer); spatialBuffer = NULL; }
    allocatedCapacity = 0;
}
