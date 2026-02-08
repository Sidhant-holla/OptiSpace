#ifndef AGENT_H
#define AGENT_H

#include "lib/include/raylib.h"  // Needed for Vector2 and Color

// The Data Structure
typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    float speed;
    Color color;
    float sepVx;
    float sepVy;
} Agent;

void InitAgents(Agent agents[], int count);
void ComputePerception(Agent agents[], int count);
void UpdatePhysics(Agent agents[], int count, Vector2 targetPos, float dt);

#endif