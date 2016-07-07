#ifndef EPUCK_APPROXIMATOR_H
#define EPUCK_APPROXIMATOR_H

#include <hal/hal.h>
#include "sensors.h"

typedef struct ApproxInputs {
    LPS_Data* lps;
    double motor_left;
    double motor_right;
} ApproxInputs;

typedef struct ApproxLocals {
    double x;
    double y;
    double phi;
    double prev_motor_left;
    double prev_motor_right;
    hal_time prev_t;
} ApproxLocals;

typedef struct ApproxState {
    ApproxLocals locals;
    struct {
        double x;
        double y;
        double phi;
    } current; /* output */
    int lps_initialized;
} ApproxState;

void approx_reset(ApproxState* approx);
void approx_step(ApproxInputs* inputs, ApproxState* approx);

#endif /* EPUCK_APPROXIMATOR_H */
