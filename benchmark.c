#include "agent.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

static double GetTimeMs(void) {
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)freq.QuadPart * 1000.0;
}

void RunBenchmark(int agentCount, int frames) {
    Agent* agents = (Agent*)malloc(sizeof(Agent) * agentCount);
    srand(42);
    InitAgents(agents, agentCount);

    float targetX = (float)(ScreenWidth / 2);
    float targetY = (float)(ScreenHeight / 2);
    float dt = 1.0f / 60.0f;

    // --- Brute Force ---
    double startBF = GetTimeMs();
    for (int f = 0; f < frames; f++) {
        ComputePerception(agents, agentCount, 0);
        UpdatePhysics(agents, agentCount, targetX, targetY, dt);
    }
    double bfTime = GetTimeMs() - startBF;

    // Re-init with same seed so kdtree gets identical starting positions
    srand(42);
    InitAgents(agents, agentCount);

    // --- KD Tree ---
    double startKD = GetTimeMs();
    for (int f = 0; f < frames; f++) {
        ComputePerception(agents, agentCount, 1);
        UpdatePhysics(agents, agentCount, targetX, targetY, dt);
    }
    double kdTime = GetTimeMs() - startKD;

    double bfMs = bfTime / frames;
    double kdMs = kdTime / frames;
    double speedup = bfMs / kdMs;

    printf("  Agents: %6d | Frames: %d\n", agentCount, frames);
    printf("  Brute Force: %8.2f ms/frame\n", bfMs);
    printf("  KD Tree:     %8.2f ms/frame\n", kdMs);
    printf("  Speedup:     %8.2fx\n\n", speedup);

    CleanupAgents();
    free(agents);
}

int main(void) {
    printf("=== OptiSpace Benchmark ===\n\n");

    int tests[] = { 1000, 5000, 10000, 20000 };
    int frames[] = { 200,  100,   50,    20  };
    int numTests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < numTests; i++) {
        RunBenchmark(tests[i], frames[i]);
    }

    printf("Done.\n");
    return 0;
}
