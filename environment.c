#include "environment.h"
#include <math.h>

void InitEnvironment(Environment* env) {
    float rx = ROOM_X;
    float ry = ROOM_Y;
    float rw = ROOM_W;
    float rh = ROOM_H;
    float wt = WALL_THICK;
    float ew = EXIT_WIDTH;

    env->roomBounds = (Rectangle){ rx, ry, rw, rh };

    int w = 0;

    // Outer walls
    env->walls[w++].rect = (Rectangle){ rx, ry, rw, wt };                          // top
    env->walls[w++].rect = (Rectangle){ rx, ry, wt, rh };                          // left
    env->walls[w++].rect = (Rectangle){ rx + rw - wt, ry, wt, rh };               // right

    // Bottom wall split by exit gap
    float exitCenterX = rx + rw / 2.0f;
    float blW = (exitCenterX - ew / 2.0f) - rx;
    env->walls[w++].rect = (Rectangle){ rx, ry + rh - wt, blW, wt };              // bottom-left
    float brX = exitCenterX + ew / 2.0f;
    float brW = (rx + rw) - brX;
    env->walls[w++].rect = (Rectangle){ brX, ry + rh - wt, brW, wt };             // bottom-right

    // Interior pillars — create corridors and bottlenecks
    env->walls[w++].rect = (Rectangle){ rx + rw * 0.18f, ry + rh * 0.18f, 140, 220 };   // upper-left
    env->walls[w++].rect = (Rectangle){ rx + rw * 0.75f, ry + rh * 0.18f, 140, 220 };   // upper-right
    env->walls[w++].rect = (Rectangle){ rx + rw * 0.32f, ry + rh * 0.58f, 120, 180 };   // lower-left (bottleneck)
    env->walls[w++].rect = (Rectangle){ rx + rw * 0.62f, ry + rh * 0.58f, 120, 180 };   // lower-right (bottleneck)

    env->wallCount = w;

    // Exit zone centered in the bottom gap
    env->exit.x = exitCenterX;
    env->exit.y = ry + rh;
    env->exit.width = ew;

    // Fire source — upper center of room
    env->fire.x = rx + rw / 2.0f;
    env->fire.y = ry + 200.0f;
    env->fire.radius = FIRE_RADIUS;
}

void DrawEnvironment(const Environment* env) {
    // Floor
    DrawRectangleRec(env->roomBounds, (Color){ 45, 42, 38, 255 });

    // Walls & pillars
    for (int i = 0; i < env->wallCount; i++) {
        Rectangle r = env->walls[i].rect;
        // Pillars (indices 5+) get a slightly different look
        if (i >= 5) {
            DrawRectangleRec(r, (Color){ 80, 75, 70, 255 });
            DrawRectangleLinesEx(r, 2, (Color){ 100, 95, 88, 255 });
        } else {
            DrawRectangleRec(r, (Color){ 90, 85, 78, 255 });
        }
    }

    // Exit highlight
    float exW = env->exit.width;
    float exX = env->exit.x - exW / 2.0f;
    float exY = env->roomBounds.y + env->roomBounds.height - WALL_THICK;
    DrawRectangle((int)exX, (int)exY, (int)exW, (int)WALL_THICK, (Color){ 50, 205, 50, 255 });
    int tw = MeasureText("EXIT", 20);
    DrawText("EXIT", (int)(env->exit.x - tw / 2), (int)(exY + WALL_THICK + 4), 20, (Color){ 50, 205, 50, 255 });

    // Fire — pulsing glow + solid core
    float pulse = 1.0f + 0.15f * sinf((float)GetTime() * 5.0f);
    DrawCircle((int)env->fire.x, (int)env->fire.y, env->fire.radius * 3.0f * pulse, (Color){ 255, 60, 0, 15 });
    DrawCircle((int)env->fire.x, (int)env->fire.y, env->fire.radius * 2.0f * pulse, (Color){ 255, 80, 0, 30 });
    DrawCircleGradient((int)env->fire.x, (int)env->fire.y, env->fire.radius, RED, ORANGE);
    DrawCircleLines((int)env->fire.x, (int)env->fire.y, FleeRadius, (Color){ 255, 50, 50, 40 });
    int fw = MeasureText("FIRE", 16);
    DrawText("FIRE", (int)(env->fire.x - fw / 2), (int)(env->fire.y - 8), 16, WHITE);
}
