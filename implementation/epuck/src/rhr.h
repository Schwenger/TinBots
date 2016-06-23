#ifndef RHR_H
#define RHR_H

#include "hal.h"
#include "sensors.h"

typedef struct RhrLocals {
    int state;
    int wall_p;
    double wall_rot;
    hal_time time_entered;
} RhrLocals;

void rhr_reset(RhrLocals* rhr);
void rhr_step(RhrLocals* rhr, Sensors* sens);

#endif
