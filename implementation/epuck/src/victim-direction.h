
#ifndef EPUCK_VICTIM_DIRECTION_H
#define EPUCK_VICTIM_DIRECTION_H

#include "sensors.h"

typedef struct VDInputs {
    int ir;
    int run_finder;
} VDInputs;

typedef struct VDLocals {
    double initial_phi;
    double bound1;
    int sensor;
} VDLocals;

typedef struct {
    int state;
    VDLocals locals;
    int motor_left;
    int motor_right;
    int victim_found;
    double victim_phi;
} VDState;

void vd_reset(VDState* vd);
void vd_step(VDInputs* inputs, VDState* vd, Sensors* sens);

#endif /* EPUCK_VICTIM_DIRECTION_H */
