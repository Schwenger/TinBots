#ifndef PATH_EXEC_H
#define PATH_EXEC_H

#include "hal.h"
#include "sensors.h"

typedef struct PathExecInputs {
    int drive_p;
    double next_x;
    double next_y;
} PathExecInputs;

typedef struct PathExecLocals {
    int state;
    hal_time time_entered;
    double start_x;
    double start_y;
    double need_rot;
    double need_dist;
    double desired_angle;
} PathExecLocals;

typedef struct PathExecState {
    int done;
    PathExecLocals locals;
} PathExecState;

void pe_reset(PathExecState* rhr);
void pe_step(PathExecInputs* inputs, PathExecState* pe, Sensors* sens);

#endif
