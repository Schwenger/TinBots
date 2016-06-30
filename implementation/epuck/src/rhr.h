#ifndef RHR_H
#define RHR_H

#include "hal.h"
#include "sensors.h"

typedef struct RhrState {
    int state;
    int wall_p;
    double wall_rot;
    hal_time time_entered;
} RhrState;

void rhr_reset(RhrState* rhr);
void rhr_step(RhrState* rhr, Sensors* sens);

#endif
