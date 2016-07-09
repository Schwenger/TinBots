#ifndef HAL_MAP_HEAP_H
#define HAL_MAP_HEAP_H

#include "map.h"

/* Typedef is already in map.h */
struct Map {
    unsigned char* data;
    int width;
    int height;
};

typedef struct MapContainer {
    Map* accu;
    Map* prox;
    Map view_buffer;
} MapContainer;

/* Must be initialized by whoever links to this file!
 * (matlab version and testing) */
extern MapContainer* map_heap_container;

Map* map_heap_alloc(int w, int h);
void map_heap_free(Map* map);

#endif
