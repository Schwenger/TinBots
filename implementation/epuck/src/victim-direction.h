
#ifndef EPUCK_VICTIM_DIRECTION_H
#define EPUCK_VICTIM_DIRECTION_H

#include "sensors.h"

typedef struct VDLocals {
    hal_time time_begin;
    hal_time counter_total;
    hal_time counter_on;
    /* Here it is really important that we use double, not float. */
    double weighted_sum;
    int sensor_id;
} VDLocals;

typedef struct VDState {
    int state;
    VDLocals locals;
    int victim_found;
    int give_up;
    double victim_phi;
} VDState;

void vd_reset(VDState* vd);
void vd_step(VDState* vd, Sensors* sens);

#endif /* EPUCK_VICTIM_DIRECTION_H */
