#ifndef EPUCK_VICTIM_FINDER_H
#define EPUCK_VICTIM_FINDER_H

#include "sensors.h"

typedef struct VFInputs {
    double victim_angle;
    int found_victim_phi;
} VFInputs;

typedef struct VFLocals {
    double data[6];
    int state;
} VFLocals;

typedef struct {
    VFLocals locals;
    int found_victim_xy;
    double victim_x;
    double victim_y;
} VFState;

void vf_reset(VFState* vf);
void vf_step(VFInputs* inputs, VFState* vf, Sensors* sens);

#endif /* EPUCK_VICTIM_FINDER_H */
