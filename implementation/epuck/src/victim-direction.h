
#ifndef EPUCK_VICTIM_DIRECTION_H
#define EPUCK_VICTIM_DIRECTION_H

#include "sensors.h"

typedef struct VDLocals {
    double initial_phi;
    double bound1;
    int sensor;
} VDLocals;

typedef struct VDState {
    int state;
    VDLocals locals;
    int victim_found;
    double victim_phi;
} VDState;

void vd_reset(VDState* vd);
void vd_step(VDState* vd, Sensors* sens);

#endif /* EPUCK_VICTIM_DIRECTION_H */
