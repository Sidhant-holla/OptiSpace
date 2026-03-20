#ifndef AGENT_H
#define AGENT_H

typedef struct {
    float x, y;
    float vx, vy;
    float speed;
    float sepVx, sepVy;
    int neighbourCount;
} Agent;

void InitAgents(Agent agents[], int count);
void ComputePerception(Agent agents[], int count);
void UpdatePhysics(Agent agents[], int count, float targetX, float targetY, float dt);

#endif
