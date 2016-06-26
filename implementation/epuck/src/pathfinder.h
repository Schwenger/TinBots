
#include "map.h"
#include "astar/astar.h"

#ifndef EPUCK_PATHFINDER_H
#define EPUCK_PATHFINDER_H

#define MAX_PATH_LENGTH 2 /* map size over const dist */
#define STEP_DISTANCE 4

typedef struct Context {
	Map *map;
	Position goal;
} Context;

Position* pf_find_path(Position position, Position goal, Map *map, Position *path);

static void find_neighbours(ASNeighborList neighbours, void *node, void *map);
static float heuristic(void *node, void *goal, void *map);
static int search_node_comparator(void *node1, void *node2, void *map);
static Position intersection(Position origin, Position goal, Map *map);

static int invalid_pos(Position pos, Map *map) {
    return pos.x >= map->width || pos.x < 0 || pos.y >= map->height || pos.y < 0;
}
static int occupied(Position *pos, Map *map) {
    return map->occupancy[pos->x][pos->y] != 0;
}

#endif /*EPUCK_PATHFINDER_H*/
