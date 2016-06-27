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

Position* pf_find_path(Position position, Position goal, Map *map, Position *path);

#endif /*EPUCK_PATHFINDER_H*/
