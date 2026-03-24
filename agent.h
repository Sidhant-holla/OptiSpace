#ifndef AGENT_H
#define AGENT_H

typedef struct {
    float x, y;
    float vx, vy;
    float speed;
    float sepVx, sepVy;
    float alignVx, alignVy;
    int neighbourCount;
} Agent;

void InitAgents(Agent agents[], int count);
void ComputePerception(Agent agents[], int count, int useKDTree);
void UpdatePhysics(Agent agents[], int count, float targetX, float targetY, float dt);
void CleanupAgents(void);

#endif
