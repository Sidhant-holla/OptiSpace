#include <stdio.h>
#include <math.h>
#include "lib/include/raylib.h"
#include <time.h>
#include <stdlib.h>
#include "config.h"
#include "agent.h"

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

    

