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
    int useKDTree = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 mousePos = GetMousePosition();

        if (IsKeyPressed(KEY_T)) useKDTree = !useKDTree;

        ComputePerception(Point, AgentCount, useKDTree);
        UpdatePhysics(Point, AgentCount, mousePos.x, mousePos.y, dt);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < AgentCount; i++) {
            Color c = Point[i].neighbourCount > 0 ? RED : BLUE;
            DrawCircle((int)Point[i].x, (int)Point[i].y, Radius, c);
        }
        DrawRectangle(0, 0, ScreenWidth, UIHeight, RAYWHITE);
        DrawText(useKDTree ? "Mode: KD Tree [T to toggle]" : "Mode: Brute Force [T to toggle]", 10, 15, 20, DARKGRAY);
        DrawFPS(10, 35);
        EndDrawing();
    }

    CleanupAgents();
    CloseWindow();
    return 0;
}
