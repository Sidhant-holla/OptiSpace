#include <stdio.h>
#include <omp.h> // The OpenMP Library
#include "lib/include/raylib.h"

int main() {
    // 1. Console Test (Check the Terminal)
    printf("Testing OpenMP...\n");
    
    // This block runs in parallel!
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        printf("Hello from Thread %d!\n", id);
    }

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