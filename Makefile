# 1. Compiler and Flags
CC = gcc
CFLAGS = -O1 -Wall -std=c99 -Wno-missing-braces -fopenmp
INCLUDES = -I lib/include
LIBS = -L lib/lib -lraylib -lopengl32 -lgdi32 -lwinmm

# 2. Files
TARGET = simulation.exe
SOURCES = main.c agent.c
# Later, you will change this to: SOURCES = main.c engine.c physics.c

# 3. Build Rules
all:
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(INCLUDES) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm $(TARGET)