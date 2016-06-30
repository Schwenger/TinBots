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
} PathFinderInputs;

typedef struct PathFinderLocals {
    Position path[MAX_PATH_LENGTH + 1];
    int path_index;
    Map* map;
} PathFinderLocals;

typedef struct PathFinderState {
    int state;
    PathFinderLocals locals;
    Position next;
    int no_path;
    int path_completed;
} PathFinderState;

void pf_reset(PathFinderState* pf);
void pf_step(PathFinderInputs* inputs, PathFinderState* pf, Sensors* sens);

#endif /*EPUCK_PATHFINDER_H*/
