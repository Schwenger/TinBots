#ifndef EPUCK_MAP_H
#define EPUCK_MAP_H

/* Beware: we probably have to copy this *during an ISR*,
 * so keep it as small as possible!
 * See map_deserialize() below. */
#define MAP_PROXIMITY_SIZE 16
#define BIT_PER_FIELD 2

typedef enum FieldType {
    FIELD_UNKNOWN, /* Should be the first element so that memset() works well. */
    FIELD_FREE,
    FIELD_WALL,
    NUM_FIELD
} FieldType;
typedef char check_map_bit_length[(NUM_FIELD <= (1 << BIT_PER_FIELD)) ? 1 : -1];

typedef struct Map Map;
Map* map_get_accumulated(void);
Map* map_get_proximity(void);
int map_get_width(Map* map);
int map_get_height(Map* map);
FieldType map_get_field(Map* map, int x, int y);
void map_set_field(Map* map, int x, int y, FieldType type);

/* TODO: If clearing a map by hand is too slow: void map_reset(Map* map); */

typedef struct Position {
    int x;
    int y;
} Position;

typedef struct ExactPosition {
    double x, y;
} ExactPosition;


/* Use this when calling map_[de]serialize.
 * There already exists a check in map_common.c whether this is up-to-date. */
#define MAP_PROXIMITY_BUF_SIZE 64

/* CAVEAT: these only return VIEWS of the respective data, so do not
 * modify the source until done, and copy your data soon!
 * Also, don't call free. on the result. */
unsigned char* map_serialize(Map* map);
Map* map_deserialize(unsigned char* buffer);
void map_clear(Map* buffer);


/* For "internal" usage (by map_static.c and map_heap.c)
 * Must be a macro, as some platforms (E-Puck) may choose to use this as
 * the size of a static array.
 * 
 * Arguments will be evaluated multiple times.
 * 
 * How it works:
 * - if w or h are not sane, require a length of -1, which should break compilation.
 * - otherwise, compute the number of bits and round up. */
#define MAP_INTERNAL_DATA_SIZE(w,h) ( \
    ((w) <= 0 || (h) <= 0 || (w) >= 256 || (h) >= 256) ? (long)-1 : \
    (((long)(w)) * ((long)(h)) * (long)BIT_PER_FIELD + (long)7) / 8 \
    )

#endif /* EPUCK_MAP_H */