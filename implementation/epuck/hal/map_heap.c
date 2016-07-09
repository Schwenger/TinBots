#include <assert.h>
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memset */

#include "map_heap.h"

/* ===== Implementation of map.h, using the heap ===== */

Map* map_get_accumulated() {
    return map_heap_container->accu;
}

Map* map_get_proximity() {
    return map_heap_container->prox;
}

int map_get_width(Map* map) {
    return map->width;
}

int map_get_height(Map* map) {
    return map->height;
}

unsigned char* map_serialize(Map* map) {
    return map->data;
}

Map* map_deserialize(unsigned char* buffer) {
    map_heap_container->view_buffer.width = MAP_PROXIMITY_SIZE;
    map_heap_container->view_buffer.height = MAP_PROXIMITY_SIZE;
    map_heap_container->view_buffer.data = buffer;
    return &map_heap_container->view_buffer;
}


/* ===== Implementation of map_heap.h ===== */

MapContainer* map_heap_container;

Map* map_heap_alloc(int w, int h) {
    Map* map = malloc(sizeof(Map));
    map->data = malloc((unsigned long)MAP_INTERNAL_DATA_SIZE(w,h));
    assert(0 == FIELD_UNKNOWN);
    memset(map->data, 0, (unsigned long)MAP_INTERNAL_DATA_SIZE(w,h));
    map->width = w;
    map->height = h;
    return map;
}

void map_heap_free(Map* map) {
    free(map->data);
    map->data = NULL;
    free(map);
}
