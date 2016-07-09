#include "map.h"

/* ===== Implementation of map.h, using the static section (.bss) ===== */

#define MAP_WIDTH 100
#define MAP_HEIGHT 100

static unsigned char map_accu[MAP_INTERNAL_DATA_SIZE(MAP_WIDTH,MAP_HEIGHT)];
static unsigned char map_prox[MAP_INTERNAL_DATA_SIZE(MAP_PROXIMITY_SIZE,MAP_PROXIMITY_SIZE)];

/* struct Map;
 * THERE IS NO MAP */

static Map* as_map(unsigned char* data) {
    return (Map*)(void*)data;
}

Map* map_get_accumulated() {
    return as_map(map_accu);
}

Map* map_get_proximity() {
    return as_map(map_prox);
}

int map_get_width(Map* map) {
    return map == map_get_accumulated() ? MAP_WIDTH : MAP_PROXIMITY_SIZE;
}

int map_get_height(Map* map) {
    return map == map_get_accumulated() ? MAP_HEIGHT : MAP_PROXIMITY_SIZE;
}

Map* map_deserialize(unsigned char* buffer) {
    return as_map(buffer);
}

unsigned char* map_serialize(Map* map) {
    return (unsigned char*)(void*)map;
}
