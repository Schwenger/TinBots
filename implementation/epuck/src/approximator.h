#ifndef EPUCK_APPROXIMATOR_H
#define EPUCK_APPROXIMATOR_H

#include <hal/hal.h>
#include "sensors.h"

typedef struct ApproxState {
    /* None of these are outputs */
    double prev_motor_left;
    double prev_motor_right;
    hal_time prev_t;
} ApproxState;

void approx_reset(ApproxState* approx, Sensors* sens);
void approx_step(ApproxState* approx, Sensors* sens);

#endif /* EPUCK_APPROXIMATOR_H */
