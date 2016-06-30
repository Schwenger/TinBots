#ifndef EPUCK_MAP_H
#define EPUCK_MAP_H

#include "pi.h"

typedef struct Map {
    const int width; /* byte probably */
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

enum {
    FIELD_FREE,
    FIELD_WALL,
    FIEL_UNKOWN
};

Position map_discretize(Map* map, double x, double y);
void send_map(Map* map, Position center, int radius);

#endif /* EPUCK_MAP_H */
