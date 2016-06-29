#ifndef EPUCK_VICTIM_FINDER_H
#define EPUCK_VICTIM_FINDER_H

#include "sensors.h"

typedef struct VFInputs {
    int found_victim_phi;
    double victim_angle;
} VFInputs;

typedef struct VFLocals {
    double data[6];
} VFLocals;

typedef struct {
    int state;
    VFLocals locals;
    int found_victim_xy;
    int victim_x;
    int victim_y;
} VFState;

void vf_reset(VFState* vf);
void vf_step(VFInputs* inputs, VFState* vf, Sensors* sens);

#endif /* EPUCK_VICTIM_FINDER_H */
