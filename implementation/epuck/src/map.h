
#ifndef EPUCK_MAP_H
#define EPUCK_MAP_H

typedef char byte;

static const int BIT_PER_FIELD = 2;

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

void map_serialize(int* data, byte* buffer, unsigned int num_fields);

void map_deserialize(int* buffer, byte* data, unsigned int num_bytes);

#endif /* EPUCK_MAP_H */
