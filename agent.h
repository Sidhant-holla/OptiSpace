#ifndef AGENT_H
#define AGENT_H

// Spatial query modes
#define MODE_BRUTE  0
#define MODE_KDTREE 1
#define MODE_COUNT  2

typedef struct {
    float x, y;
    float vx, vy;
    float speed;
    float sepVx, sepVy;
    float alignVx, alignVy;
    int neighbourCount;
    int active;
} Agent;

void InitAgents(Agent agents[], int count);
void ComputePerception(Agent agents[], int count, int spatialMode);
void UpdatePhysics(Agent agents[], int count, float fireX, float fireY, float exitX, float exitY, float dt);
void CleanupAgents(void);

#endif
