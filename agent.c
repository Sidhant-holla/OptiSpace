// Agent behavior uses Reynolds flocking model with three forces:
//   Flee       - repel from cursor 
//   Separation - repel from nearby neighbors (inverse-square)
//   Alignment  - steer toward average neighbor velocity
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

void InitAgents(Agent agents[], int count) {
    if (spatialBuffer) free(spatialBuffer);
    spatialBuffer = (SpatialPoint*)malloc(sizeof(SpatialPoint) * count);
    for (int i = 0; i < count; i++) {
        agents[i].x = Radius + rand() % (ScreenWidth - 2 * Radius);
        agents[i].y = UIHeight + Radius + rand() % (ScreenHeight - UIHeight - 2 * Radius);
        agents[i].vx = 0.0f;
        agents[i].vy = 0.0f;
        agents[i].speed = (500 + rand() % ((int)(MaxSpeed * 100) - 500)) / 100.0f;
        agents[i].sepVx = 0.0f;
        agents[i].sepVy = 0.0f;
        agents[i].neighbourCount = 0;
        agents[i].alignVx = 0.0f;
        agents[i].alignVy = 0.0f;
    }
}

void ComputePerception(Agent agents[], int count, int useKDTree) {
    for (int i = 0; i < count; i++) {
        spatialBuffer[i].x = agents[i].x;
        spatialBuffer[i].y = agents[i].y;
        spatialBuffer[i].index = i;
    }

    if (useKDTree) {
        if (!tree) tree = InitKDTree(count);
        RebuildKDTree_InPlace(tree, spatialBuffer, count);
    }

    for (int i = 0; i < count; i++) {
        int neighbourCount = 0;
        agents[i].sepVx = 0;
        agents[i].sepVy = 0;
        agents[i].alignVx = 0;
        agents[i].alignVy = 0;

        int results[MaxNeighbours];

        if (useKDTree) {
            QueryKDTree(tree->root, agents[i].x, agents[i].y, NeighbourRadius, results, &neighbourCount, MaxNeighbours);
        } else {
            QueryBruteForce(spatialBuffer, count, agents[i].x, agents[i].y, NeighbourRadius, results, &neighbourCount, MaxNeighbours);
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

void UpdatePhysics(Agent agents[], int count, float targetX, float targetY, float dt) {
    for (int i = 0; i < count; i++) {
        float dx = agents[i].x - targetX;
        float dy = agents[i].y - targetY;
        float length = sqrtf(dx * dx + dy * dy);

        float fleeVx = 0;
        float fleeVy = 0;

        if (length > 0.001f && length < FleeRadius) {
            fleeVx = (dx / length) * agents[i].speed;
            fleeVy = (dy / length) * agents[i].speed;
        }

        float separationVx = agents[i].sepVx * SepMultiplier;
        float separationVy = agents[i].sepVy * SepMultiplier;
        float alignVx = agents[i].alignVx * AlignMultiplier;
        float alignVy = agents[i].alignVy * AlignMultiplier;

        float desiredVx = fleeVx + separationVx + alignVx;
        float desiredVy = fleeVy + separationVy + alignVy;

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

        if (agents[i].x > ScreenWidth - Radius) { agents[i].x = ScreenWidth - Radius; agents[i].vx *= -1; }
        if (agents[i].x < Radius) { agents[i].x = Radius; agents[i].vx *= -1; }
        if (agents[i].y > ScreenHeight - Radius) { agents[i].y = ScreenHeight - Radius; agents[i].vy *= -1; }
        if (agents[i].y < UIHeight + Radius) { agents[i].y = UIHeight + Radius; agents[i].vy *= -1; }
    }
}

void CleanupAgents(void) {
    if (tree) { FreeKDTree(tree); tree = NULL; }
    if (spatialBuffer) { free(spatialBuffer); spatialBuffer = NULL; }
}
