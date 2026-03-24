#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "lib/include/raylib.h"
#include "config.h"

#define MAX_WALLS 16

typedef struct {
    Rectangle rect;
} Wall;

typedef struct {
    float x, y;
    float width;
} ExitZone;

typedef struct {
    float x, y;
    float radius;
} FireSource;

typedef struct {
    Wall walls[MAX_WALLS];
    int wallCount;
    ExitZone exit;
    FireSource fire;
    Rectangle roomBounds;
} Environment;

void InitEnvironment(Environment* env);
void DrawEnvironment(const Environment* env);

#endif
