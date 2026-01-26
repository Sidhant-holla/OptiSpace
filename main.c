#include <stdio.h>
#include <math.h>
#include "lib/include/raylib.h"
#include <time.h>
#include <stdlib.h>

#define ScreenWidth 800
#define ScreenHeight 600
#define Radius 5
#define AgentCount 100

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    Color color;
} Agent;

int main() {
    InitWindow(ScreenWidth, ScreenHeight, "Simulation");
    SetTargetFPS(60);
    srand(time(NULL));

    Agent Point[AgentCount];
    for (int i = 0; i < AgentCount; i++){
        Point[i].x = GetRandomValue(Radius, ScreenWidth-Radius);
        Point[i].y = GetRandomValue(Radius, ScreenHeight-Radius);
        Point[i].vx = GetRandomValue(-40, 40) * 0.1f;
        Point[i].vy = GetRandomValue(-40, 40) * 0.1f;
        Point[i].color = RED;
    }

    while (!WindowShouldClose()) {
        Vector2 CursorPos = GetMousePosition();
        for (int i = 0; i < AgentCount; i++){
            Point[i].vx = ((CursorPos.x + GetRandomValue(-2000, 2000)*0.1f) - Point[i].x) * 0.04f;
            Point[i].vy = ((CursorPos.y + GetRandomValue(-2000, 2000)*0.1f) - Point[i].y) * 0.04f;
            Point[i].x += Point[i].vx;
            Point[i].y += Point[i].vy;
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < AgentCount; i++){
            DrawCircle(Point[i].x, Point[i].y, 5, Point[i].color);
        }
        DrawText("Move your cursor!", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
