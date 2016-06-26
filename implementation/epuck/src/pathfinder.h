//
// Created by Maximilian Schwenger on 19/06/16.
//

#include "astar/astar.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef EPUCK_PATHFINDER_H
#define EPUCK_PATHFINDER_H

#define MAX_PATH_LENGTH 2 // map size over const dist
#define STEP_DISTANCE 4

typedef struct Position {
    int x, y;
} Position;

// aka context
typedef struct Map {
	const int width, height;
	int** occupancy;
	Position goal;
} Map;

int main(void);

static const Position NO_PATH = {-1, -1};

Position* find_path(Position position, Position goal, Map *map, Position *path);

void find_neighbours(ASNeighborList neighbours, void *node, void *map);

float heuristic(void *node, void *goal, void *map);

int search_node_comparator(void *node1, void *node2, void *map);

Position intersection(Position origin, Position goal, Map *map);

int invalid_pos(Position pos, Map *map) {
	return pos.x >= map->width || pos.x < 0 || pos.y >= map->height || pos.y < 0;
}

int occupied(Position *pos, Map *map) {
	return map->occupancy[pos->x][pos->y] != 0;
}

#endif //EPUCK_PATHFINDER_H
