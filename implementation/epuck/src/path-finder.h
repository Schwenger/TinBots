#ifndef EPUCK_PATHFINDER_H
#define EPUCK_PATHFINDER_H

#include <hal/hal.h>
#include "map.h"
#include "sensors.h"

#define STEP_DISTANCE 4
#define MAX_PATH_LENGTH 20 /* map size over const dist */
/* Caller must allocate space for MAX_PATH_LENGTH+1 instances of Position. */

typedef struct PathFinderInputs {
    double dest_x;
    double dest_y;
    int compute;
    int step_complete;
    int step_see_obstacle;
    int is_victim;
} PathFinderInputs;

typedef struct PathFinderLocals {
    Map* map;
    Position path[MAX_PATH_LENGTH + 1];
    int path_index; /* Must be signed */
    int state;
} PathFinderLocals;

typedef struct PathFinderState {
    PathFinderLocals locals;
    Position next;
    int no_path;
    int path_completed;
    int drive;
    int backwards;
} PathFinderState;

void pf_reset(PathFinderState* pf);
void pf_step(PathFinderInputs* inputs, PathFinderState* pf, Sensors* sens);

/* Only for testing */
void pf_find_path(Position position, Position goal, Map *map, Position *path);

#endif /*EPUCK_PATHFINDER_H*/
