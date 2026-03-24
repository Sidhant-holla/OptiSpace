#ifndef CONFIG_H
#define CONFIG_H

// --- Window ---
#define ScreenWidth 2560
#define ScreenHeight 1440
#define TargetFPS 60

// --- UI Panel ---
#define PANEL_W 280

// --- Room ---
#define ROOM_X 320.0f
#define ROOM_Y 40.0f
#define ROOM_W 2200.0f
#define ROOM_H 1360.0f
#define WALL_THICK 12.0f
#define EXIT_WIDTH 180.0f
#define FIRE_RADIUS 50.0f

// --- Agent ---
#define INITIAL_AGENT_COUNT 20000
#define AGENT_STEP 100
#define Radius 5
#define MaxSpeed 200.0f
#define MaxForce 400.0f

// --- Query ---
#define MaxNeighbours 512

// --- Behavior ---
#define FleeRadius 300.0f
#define NeighbourRadius 40.0f
#define SepMultiplier 1500.0f
#define SteeringForce 6.0f
#define AlignMultiplier 0.1f
#define ExitAttraction 180.0f

// --- Crowding ---
#define CROWD_THRESHOLD 8

#endif
