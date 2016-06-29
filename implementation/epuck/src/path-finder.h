#ifndef EPUCK_PATHFINDER_H
#define EPUCK_PATHFINDER_H

typedef struct Map {
    const int width;
    const int height;
    int** occupancy;
} Map;

typedef struct Position {
    int x;
    int y;
} Position;

typedef struct ExactPosition {
    double x, y;
} ExactPosition;

#define STEP_DISTANCE 4
#define MAX_PATH_LENGTH 20 /* map size over const dist */
/* Caller must allocate space for MAX_PATH_LENGTH+1 instances of Position. */
void pf_find_path(Position position, Position goal, Map *map, Position *path);

#endif /*EPUCK_PATHFINDER_H*/
