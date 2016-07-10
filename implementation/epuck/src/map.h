#ifndef EPUCK_MAP_H
#define EPUCK_MAP_H

/* Beware: we probably have to copy this *during an ISR*,
 * so keep it as small as possible!
 * There are multiple restrictions on this value (due to optimizations),
 * so keep it a multiple of 4. */
#define MAP_PROXIMITY_SIZE 16

/* This is not configurable.
 * Seriously, map_merge becomes impossible for higher values. */
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
/* by_x must be a multiple of 4. */
void map_move(Map* buffer, int by_x, int by_y);
void map_clear(Map* buffer);
/* Highly optimized, can be called inside an ISR: */
void map_merge(Map* dst, int low_left_x, int low_left_y, Map* patch);

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
    ((w) <= 0 || (h) <= 0 || (w) >= 256 || (h) >= 256 || (w)%4 != 0 || (h)%2 != 0) ? (long)-1 : \
    (((long)(w)) * ((long)(h)) * (long)BIT_PER_FIELD + (long)7) / 8 \
    ) /* (w*h*bits + 7)/8 */

/* For testing */
unsigned long map_internal_locate(int x, int y, int w, int h);

#endif /* EPUCK_MAP_H */
