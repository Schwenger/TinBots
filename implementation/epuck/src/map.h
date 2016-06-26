#ifndef EPUCK_MAP_H
#define EPUCK_MAP_H

typedef struct Map {
    const int width, height;
    int** occupancy;
} Map;

typedef struct Position {
    int x, y;
} Position;

static const Position INVALID_POS = {-1, -1};

#endif /* EPUCK_MAP_H */
