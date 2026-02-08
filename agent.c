#include "agent.h"
#include "config.h"
#include <math.h>
#include <stdlib.h> 

void InitAgents(Agent agents[], int count) {
    for (int i = 0; i < count; i++) {
        agents[i].x = GetRandomValue(Radius, ScreenWidth - Radius);
        agents[i].y = GetRandomValue(Radius, ScreenHeight - Radius);
        agents[i].vx = 0.0f;
        agents[i].vy = 0.0f;
        agents[i].speed = GetRandomValue(50, (int)(MaxSpeed * 100)) / 100.0f;
        agents[i].color = BLUE;
        agents[i].sepVx = 0.0f;
        agents[i].sepVy = 0.0f;
    }
}

void ComputePerception(Agent Point[], int count) {
    float radiusSquare = NeighbourRadius * NeighbourRadius;

    for (int i = 0; i < count; i++) {
        int neighbourCount = 0;
        Point[i].color = BLUE;
        Point[i].sepVx = 0;
        Point[i].sepVy = 0;

        for (int j = 0; j < count; j++) {
            if (i == j) continue;

            float dx = Point[i].x - Point[j].x;
            float dy = Point[i].y - Point[j].y;
            float distSquare = dx * dx + dy * dy;

            if (distSquare < radiusSquare && distSquare > 0.001f) {
                neighbourCount++;
                Point[i].sepVx += dx / distSquare;
                Point[i].sepVy += dy / distSquare;
            }
        }
        if (neighbourCount > 0) {
            Point[i].color = RED;
        }
    }
}

void UpdatePhysics(Agent Point[], int count, Vector2 targetPos, float dt) {
    for (int i = 0; i < count; i++) {
        float dx = Point[i].x - targetPos.x;
        float dy = Point[i].y - targetPos.y;
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
        if (Point[i].y < Radius) { Point[i].y = Radius; Point[i].vy *= -1; }
    }
}