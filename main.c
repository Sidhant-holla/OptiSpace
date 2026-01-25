#include <stdio.h>
#include "lib/include/raylib.h"

int main() {

    // 2. Graphics Test (Check the Window)
    InitWindow(800, 450, "CrowdControl - OpenMP Ready");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("OpenMP is Active!", 300, 200, 20, DARKGREEN);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}