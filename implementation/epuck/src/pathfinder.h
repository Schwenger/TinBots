
#include "map.h"

#ifndef EPUCK_PATHFINDER_H
#define EPUCK_PATHFINDER_H

#define MAX_PATH_LENGTH 2 /* map size over const dist */
#define STEP_DISTANCE 4

typedef struct Context {
	Map *map;
	Position goal;
} Context;

Position* pf_find_path(Position position, Position goal, Map *map, Position *path);

#endif /*EPUCK_PATHFINDER_H*/
