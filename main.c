#include <stdio.h>
#include <math.h>
#include "lib/include/raylib.h"
#include <time.h>
#include <stdlib.h>
#include "config.h"
#include "agent.h"
#include "environment.h"

// ── Simple UI helpers ──────────────────────────────────────────────

static int GuiButton(int x, int y, int w, int h, const char* text, int active) {
    Rectangle r = { (float)x, (float)y, (float)w, (float)h };
    int hover = CheckCollisionPointRec(GetMousePosition(), r);
    Color bg = active ? (Color){ 60, 120, 170, 255 } : (Color){ 50, 50, 55, 255 };
    if (hover && !active) bg = (Color){ 65, 65, 72, 255 };
    DrawRectangleRec(r, bg);
    DrawRectangleLinesEx(r, 1, (Color){ 80, 80, 90, 255 });
    int tw = MeasureText(text, 16);
    DrawText(text, x + (w - tw) / 2, y + (h - 16) / 2, 16, active ? WHITE : LIGHTGRAY);
    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void SectionLabel(int y, const char* text) {
    DrawText(text, 20, y, 16, (Color){ 140, 140, 155, 255 });
}

// ── Main ───────────────────────────────────────────────────────────

int main() {
    InitWindow(ScreenWidth, ScreenHeight, "OptiSpace — Fire Evacuation");
    SetTargetFPS(TargetFPS);
    srand(time(NULL));

    int agentCount = INITIAL_AGENT_COUNT;
    int agentCapacity = agentCount;
    Agent* agents = (Agent*)malloc(sizeof(Agent) * agentCapacity);
    InitAgents(agents, agentCount);

    Environment env;
    InitEnvironment(&env);

    int spatialMode = MODE_KDTREE;
    int paused = 0;
    int escaped = 0;
    double computeMs = 0.0;
    double avgMs = 0.0;

    float speedOpts[] = { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };
    int speedIdx = 2;

    while (!WindowShouldClose()) {
        float rawDt = GetFrameTime();
        float dt = paused ? 0.0f : rawDt * speedOpts[speedIdx];

        // ── Keyboard shortcuts ──
        if (IsKeyPressed(KEY_T)) spatialMode = (spatialMode + 1) % MODE_COUNT;
        if (IsKeyPressed(KEY_SPACE)) paused = !paused;
        if (IsKeyPressed(KEY_R)) { InitAgents(agents, agentCount); escaped = 0; }

        if (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)) {
            agentCount += AGENT_STEP;
            if (agentCount > agentCapacity) {
                agentCapacity = agentCount + agentCount / 2;
                agents = (Agent*)realloc(agents, sizeof(Agent) * agentCapacity);
            }
            InitAgents(agents, agentCount); escaped = 0;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
            agentCount -= AGENT_STEP;
            if (agentCount < AGENT_STEP) agentCount = AGENT_STEP;
            InitAgents(agents, agentCount); escaped = 0;
        }

        // ── Click to place fire (only inside room, outside panel) ──
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mp = GetMousePosition();
            if (mp.x > ROOM_X + WALL_THICK && mp.x < ROOM_X + ROOM_W - WALL_THICK &&
                mp.y > ROOM_Y + WALL_THICK && mp.y < ROOM_Y + ROOM_H - WALL_THICK) {
                env.fire.x = mp.x;
                env.fire.y = mp.y;
            }
        }

        // ── Simulation step ──
        if (!paused) {
            double t0 = GetTime();
            ComputePerception(agents, agentCount, spatialMode);
            UpdatePhysics(agents, agentCount, env.fire.x, env.fire.y, env.exit.x, env.exit.y, dt);
            computeMs = (GetTime() - t0) * 1000.0;
            avgMs = avgMs * 0.93 + computeMs * 0.07;
        }

        // ── Wall collision + fire collision + exit detection ──
        for (int i = 0; i < agentCount; i++) {
            if (!agents[i].active) continue;

            // Wall / pillar collision
            for (int w = 0; w < env.wallCount; w++) {
                Rectangle wr = env.walls[w].rect;
                float ax = agents[i].x, ay = agents[i].y;
                if (ax > wr.x - Radius && ax < wr.x + wr.width + Radius &&
                    ay > wr.y - Radius && ay < wr.y + wr.height + Radius) {
                    float dL = ax - (wr.x - Radius);
                    float dR = (wr.x + wr.width + Radius) - ax;
                    float dT = ay - (wr.y - Radius);
                    float dB = (wr.y + wr.height + Radius) - ay;
                    float minD = dL; int dir = 0;
                    if (dR < minD) { minD = dR; dir = 1; }
                    if (dT < minD) { minD = dT; dir = 2; }
                    if (dB < minD) { minD = dB; dir = 3; }
                    switch (dir) {
                        case 0: agents[i].x = wr.x - Radius;            agents[i].vx = -fabsf(agents[i].vx); break;
                        case 1: agents[i].x = wr.x + wr.width + Radius; agents[i].vx =  fabsf(agents[i].vx); break;
                        case 2: agents[i].y = wr.y - Radius;            agents[i].vy = -fabsf(agents[i].vy); break;
                        case 3: agents[i].y = wr.y + wr.height + Radius; agents[i].vy = fabsf(agents[i].vy); break;
                    }
                }
            }

            // Fire collision — hard block, agents cannot enter fire radius
            {
                float fdx = agents[i].x - env.fire.x;
                float fdy = agents[i].y - env.fire.y;
                float fd = sqrtf(fdx * fdx + fdy * fdy);
                float blockR = env.fire.radius + Radius;
                if (fd < blockR && fd > 0.001f) {
                    float nx = fdx / fd;
                    float ny = fdy / fd;
                    agents[i].x = env.fire.x + nx * blockR;
                    agents[i].y = env.fire.y + ny * blockR;
                    float vdot = agents[i].vx * nx + agents[i].vy * ny;
                    if (vdot < 0) {
                        agents[i].vx -= 2.0f * vdot * nx;
                        agents[i].vy -= 2.0f * vdot * ny;
                    }
                }
            }

            // Exit detection
            if (agents[i].y > env.exit.y &&
                fabsf(agents[i].x - env.exit.x) < env.exit.width / 2.0f) {
                agents[i].active = 0;
                agents[i].x = -10000.0f;
                agents[i].y = -10000.0f;
                escaped++;
            }
        }

        // ══════════ RENDER ══════════
        BeginDrawing();
        ClearBackground((Color){ 25, 25, 28, 255 });

        // Room + walls + fire
        DrawEnvironment(&env);

        // Danger zone ring
        DrawCircleLines((int)env.fire.x, (int)env.fire.y, FleeRadius, (Color){ 255, 50, 50, 40 });

        // Agents — directional triangles, colored by crowd density
        for (int i = 0; i < agentCount; i++) {
            if (!agents[i].active) continue;

            float crowd = (float)agents[i].neighbourCount / (float)CROWD_THRESHOLD;
            if (crowd > 1.0f) crowd = 1.0f;
            Color c;
            if (crowd < 0.5f) {
                float t = crowd * 2.0f;
                c = (Color){ (unsigned char)(t * 255), (unsigned char)(200 + t * 55), (unsigned char)((1.0f - t) * 255), 255 };
            } else {
                float t = (crowd - 0.5f) * 2.0f;
                c = (Color){ 255, (unsigned char)((1.0f - t) * 255), 0, 255 };
            }

            float vx = agents[i].vx, vy = agents[i].vy;
            float vmag = sqrtf(vx * vx + vy * vy);
            float angle = (vmag > 0.1f) ? atan2f(vy, vx) : 0.0f;
            float sz = (float)Radius * 1.8f;
            float px = agents[i].x, py = agents[i].y;

            Vector2 tip   = { px + cosf(angle) * sz,          py + sinf(angle) * sz };
            Vector2 left  = { px + cosf(angle + 2.5f) * sz * 0.6f, py + sinf(angle + 2.5f) * sz * 0.6f };
            Vector2 right = { px + cosf(angle - 2.5f) * sz * 0.6f, py + sinf(angle - 2.5f) * sz * 0.6f };
            DrawTriangle(tip, left, right, c);
            DrawTriangle(tip, right, left, c); // both windings so it always renders
        }

        // ── UI Panel ──
        DrawRectangle(0, 0, PANEL_W, ScreenHeight, (Color){ 28, 28, 33, 245 });
        DrawLine(PANEL_W, 0, PANEL_W, ScreenHeight, (Color){ 55, 55, 65, 255 });

        DrawText("OptiSpace", 20, 20, 28, WHITE);
        DrawText("Fire Evacuation", 20, 52, 16, (Color){ 160, 160, 170, 255 });

        // Mode selector
        SectionLabel(95, "SPATIAL MODE");
        if (GuiButton(20, 118, 240, 30, "Brute Force",  spatialMode == MODE_BRUTE))  spatialMode = MODE_BRUTE;
        if (GuiButton(20, 152, 240, 30, "KD-Tree",      spatialMode == MODE_KDTREE)) spatialMode = MODE_KDTREE;

        // Agent count
        SectionLabel(235, "AGENTS");
        char buf[64];
        snprintf(buf, sizeof(buf), "%d", agentCount);
        int cw = MeasureText(buf, 24);
        DrawText(buf, 20 + (240 - cw) / 2, 258, 24, WHITE);
        if (GuiButton(20, 290, 115, 30, "- 100", 0)) {
            agentCount -= AGENT_STEP;
            if (agentCount < AGENT_STEP) agentCount = AGENT_STEP;
            InitAgents(agents, agentCount); escaped = 0;
        }
        if (GuiButton(145, 290, 115, 30, "+ 100", 0)) {
            agentCount += AGENT_STEP;
            if (agentCount > agentCapacity) {
                agentCapacity = agentCount + agentCount / 2;
                agents = (Agent*)realloc(agents, sizeof(Agent) * agentCapacity);
            }
            InitAgents(agents, agentCount); escaped = 0;
        }

        // Speed
        SectionLabel(340, "SPEED");
        snprintf(buf, sizeof(buf), "%.2fx", speedOpts[speedIdx]);
        int sw = MeasureText(buf, 22);
        DrawText(buf, 20 + (240 - sw) / 2, 362, 22, WHITE);
        if (GuiButton(20, 392, 56, 30, "<<", 0) && speedIdx > 0) speedIdx--;
        if (GuiButton(80, 392, 56, 30, "<", 0) && speedIdx > 0)  speedIdx--;
        if (GuiButton(144, 392, 56, 30, ">", 0) && speedIdx < 4) speedIdx++;
        if (GuiButton(204, 392, 56, 30, ">>", 0) && speedIdx < 4) speedIdx++;

        // Play / Pause / Reset
        SectionLabel(442, "CONTROLS");
        if (GuiButton(20, 465, 115, 34, paused ? "> PLAY" : "|| PAUSE", paused)) paused = !paused;
        if (GuiButton(145, 465, 115, 34, "RESET", 0)) { InitAgents(agents, agentCount); escaped = 0; }

        // Statistics
        SectionLabel(520, "STATISTICS");
        int alive = agentCount - escaped;
        snprintf(buf, sizeof(buf), "Remaining:  %d", alive);
        DrawText(buf, 20, 546, 18, WHITE);
        snprintf(buf, sizeof(buf), "Escaped:    %d", escaped);
        DrawText(buf, 20, 570, 18, (Color){ 80, 220, 80, 255 });

        float pct = agentCount > 0 ? (float)escaped / (float)agentCount * 100.0f : 0.0f;
        snprintf(buf, sizeof(buf), "Evacuated:  %.1f%%", pct);
        DrawText(buf, 20, 594, 18, (Color){ 80, 220, 80, 255 });

        // Progress bar
        DrawRectangle(20, 622, 240, 8, (Color){ 40, 40, 48, 255 });
        int barW = (int)(240.0f * pct / 100.0f);
        DrawRectangle(20, 622, barW, 8, (Color){ 80, 220, 80, 255 });

        // Performance
        SectionLabel(648, "PERFORMANCE");
        Color msColor = avgMs < 4.0 ? (Color){ 80, 220, 80, 255 } :
                         avgMs < 10.0 ? (Color){ 255, 200, 50, 255 } :
                                        (Color){ 255, 70, 70, 255 };
        snprintf(buf, sizeof(buf), "Compute:  %.2f ms", avgMs);
        DrawText(buf, 20, 674, 18, msColor);
        snprintf(buf, sizeof(buf), "FPS:      %d", GetFPS());
        DrawText(buf, 20, 698, 18, WHITE);

        // Hint
        DrawText("Click inside room", 20, ScreenHeight - 70, 15, (Color){ 100, 100, 115, 255 });
        DrawText("to reposition fire", 20, ScreenHeight - 52, 15, (Color){ 100, 100, 115, 255 });
        DrawText("[T] mode  [Space] pause", 20, ScreenHeight - 30, 13, (Color){ 80, 80, 95, 255 });

        EndDrawing();
    }

    free(agents);
    CleanupAgents();
    CloseWindow();
    return 0;
}
