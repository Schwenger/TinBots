#ifndef EPUCK_MAP_H
#define EPUCK_MAP_H

#include "pi.h"

typedef struct Map {
    int width;
    int height;
    int** occupancy;
} Map;

typedef struct Position {
    int x;
    int y;
} Position;

typedef struct ExactPosition {
    double x, y;
} ExactPosition;

enum {
    FIELD_FREE,
    FIELD_WALL,
    FIEL_UNKOWN
};

void send_map(Map* map, Position center, int radius);

#endif /* EPUCK_MAP_H */
