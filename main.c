#include <stdio.h>
#include <math.h>
#include "lib/include/raylib.h"
#include <time.h>
#include <stdlib.h>

#define ScreenWidth 1600
#define ScreenHeight 800
#define Radius 5
#define AgentCount 200
#define MaxSpeed 400.0f
#define MaxForce 400.0f
#define NeighbourRadius 50.0f

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

void ComputePerception(Agent Point[], int count){
    float radiusSquare = NeighbourRadius*NeighbourRadius;
    
    for (int i  = 0; i < count; i++){
        int neighbourCount = 0;
        Point[i].color = BLUE;
        Point[i].sepVx = 0;
        Point[i].sepVy = 0;
        for (int j = 0; j < count; j++){
            if (i == j) continue;
            float dx = Point[i].x - Point[j].x;
            float dy = Point[i].y - Point[j].y;
            float distSquare = dx*dx + dy*dy;
            if (distSquare < radiusSquare) {
                neighbourCount++;
                if (distSquare > 0.001f){
                    Point[i].sepVx += dx / distSquare;
                    Point[i].sepVy += dy / distSquare;
                }

            }    
        } 
        if (neighbourCount > 0) {
            Point[i].color = RED;
            float length = sqrtf(Point[i].sepVx*Point[i].sepVx + Point[i].sepVy*Point[i].sepVy);
            if (length > 0){
                Point[i].sepVx = (Point[i].sepVx/length) * Point[i].speed;
                Point[i].sepVy = (Point[i].sepVy/length) * Point[i].speed;
            }
        }

    }

}

void UpdatePhysics(Agent Point[], int count, Vector2 targetPos, float dt){
    for (int i = 0; i < count; i++){
        float dx = Point[i].x - targetPos.x;
        float dy = Point[i].y - targetPos.y;
        float length = sqrtf(dx*dx + dy*dy);
        float fleeVx = 0;
        float fleeVy = 0;

        if (length > 0.001f && length < 500.f){
            fleeVx = (dx/length) * Point[i].speed;
            fleeVy = (dy/length) * Point[i].speed;
        }

        float fleeWeight = 2.5f;
        float sepWeight = 4.0f;

        float desiredVx = fleeVx*fleeWeight + Point[i].sepVx*sepWeight;
        float desiredVy = fleeVy*fleeWeight + Point[i].sepVy*sepWeight;

        float steeringForce = 4.0f;
        float ax = (desiredVx - Point[i].vx) * steeringForce;
        float ay = (desiredVy - Point[i].vy) * steeringForce;

        float aMag = sqrtf(ax*ax + ay*ay);
        if (aMag > MaxForce){
            ax = (ax/aMag) * MaxForce;
            ay = (ay/aMag) * MaxForce;
        }

        Point[i].vx += dt * ax;
        Point[i].vy += dt* ay;

        float vMag = sqrtf(Point[i].vx*Point[i].vx + Point[i].vy*Point[i].vy);
        if (vMag > MaxSpeed){
            Point[i].vx = (Point[i].vx / vMag) * MaxSpeed;
            Point[i].vy = (Point[i].vy / vMag) * MaxSpeed;
        }

        Point[i].x += dt*Point[i].vx;
        Point[i].y += dt*Point[i].vy;

        if (Point[i].x > ScreenWidth - Radius) { Point[i].x = ScreenWidth - Radius; Point[i].vx *= -1; }
        if (Point[i].x < Radius) { Point[i].x = Radius; Point[i].vx *= -1; }
        if (Point[i].y > ScreenHeight - Radius) { Point[i].y = ScreenHeight - Radius; Point[i].vy *= -1; }
        if (Point[i].y < Radius) { Point[i].y = Radius; Point[i].vy *= -1; }
        

    }
}


int main() {
    InitWindow(ScreenWidth, ScreenHeight, "Simulation");
    SetTargetFPS(60);
    srand(time(NULL));

    Agent Point[AgentCount];
    for (int i = 0; i < AgentCount; i++){
        Point[i].x = GetRandomValue(Radius, ScreenWidth-Radius);
        Point[i].y = GetRandomValue(Radius, ScreenHeight-Radius);
        Point[i].vx = 0.0f;
        Point[i].vy = 0.0f;
        Point[i].speed = GetRandomValue(50, MaxSpeed*100)/100.0f;
        Point[i].color = BLUE;
    }
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 mousePos = GetMousePosition();
        ComputePerception(Point, AgentCount);
        UpdatePhysics(Point, AgentCount, mousePos, dt);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < AgentCount; i++) {
            DrawCircle((int)Point[i].x, (int)Point[i].y, Radius, Point[i].color);
        }
        DrawText("Modular Code: Sense -> Act -> Draw", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

    

