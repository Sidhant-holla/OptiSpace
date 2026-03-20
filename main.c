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
    InitAgents(Point, AgentCount);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 mousePos = GetMousePosition();
        ComputePerception(Point, AgentCount);
        UpdatePhysics(Point, AgentCount, mousePos.x, mousePos.y, dt);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < AgentCount; i++) {
            Color c = Point[i].neighbourCount > 0 ? RED : BLUE;
            DrawCircle((int)Point[i].x, (int)Point[i].y, Radius, c);
        }
        DrawText("Modular Code: Sense -> Act -> Draw", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
