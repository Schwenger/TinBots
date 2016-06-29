#ifndef EPUCK_TRAFFIC_COP_EYES_H
#define EPUCK_TRAFFIC_COP_EYES_H

#include "sensors.h"

typedef struct TCEInputs {
    int found_victim_phi;
    int found_victim_xy;
    double ray_phi;
    int ir_stable;
} TCEInputs;

typedef struct TCELocals {
    double last_x;
    double last_y;
    double last_phi;
} TCELocals;

typedef struct TCEState {
    int state;
    TCELocals locals;
    int need_angle; /* output */
} TCEState;

void tce_reset(TCEState* tce);
void tce_step(TCEInputs* inputs, TCEState* tce, Sensors* sens);

#endif /* EPUCK_TRAFFIC_COP_EYES_H */
