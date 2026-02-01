#include <stdio.h>
#include <math.h>
#include "lib/include/raylib.h"
#include <time.h>
#include <stdlib.h>

#define ScreenWidth 800
#define ScreenHeight 600
#define Radius 5
#define AgentCount 100
#define MaxSpeed 400.0f
#define MaxForce 400.0f

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    float speed;
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
        Point[i].vx = 0.0f;
        Point[i].vy = 0.0f;
        Point[i].speed = GetRandomValue(50, MaxSpeed*100)/100.0f;
        Point[i].color = RED;
    }

    while (!WindowShouldClose()) {
        Vector2 CursorPos = GetMousePosition();
        for (int i = 0; i < AgentCount; i++){
            float dx = (CursorPos.x - Point[i].x);
            float dy = CursorPos.y - Point[i].y;
            float length = sqrt((dx*dx)+(dy*dy));
            float dt = GetFrameTime();
            if(length>=0.001f){
                dx /= length;
                dy /= length;
            }
            if(Point[i].x>ScreenWidth-Radius){
                Point[i].x= ScreenWidth-Radius;
                Point[i].vx *= -1 ;
            }
            if(Point[i].x<Radius){
                Point[i].x= Radius;
                Point[i].vx *= -1 ;
            }
            if(Point[i].y>ScreenHeight-Radius){
                Point[i].y=ScreenHeight-Radius;
                Point[i].vy*=-1;
            }
            if(Point[i].y<Radius){
                Point[i].y=Radius;
                Point[i].vy*=-1;
            }
            float desiredVx = -1*dx*Point[i].speed;
            float desiredVy = -1*dy*Point[i].speed;
            int steeringForce = 4;
            float ax = steeringForce*(desiredVx - Point[i].vx);
            float ay = steeringForce*(desiredVy - Point[i].vy);
            float aMagnitude = sqrt((ax*ax)+(ay*ay));
            if(aMagnitude>MaxForce){
                ax = (ax/aMagnitude)*MaxForce;
                ay = (ay/aMagnitude)*MaxForce;
            }
            Point[i].vx += dt*ax;
            Point[i].vy += dt*ay;
            float vMagnitude = sqrt((Point[i].vx*Point[i].vx)+(Point[i].vy*Point[i].vy));
            if(vMagnitude>MaxSpeed){
                Point[i].vx = (Point[i].vx/vMagnitude)*MaxSpeed;
                Point[i].vy = (Point[i].vy/vMagnitude)*MaxSpeed;
            }
            Point[i].x += dt*Point[i].vx;
            Point[i].y += dt*Point[i].vy;
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
