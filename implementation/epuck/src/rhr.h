#ifndef RHR_H
#define RHR_H

#include "sensors.h"

typedef struct RhrLocals {
    int state; /* The state we will *enter* at the next step() */
    int wall_p;
    double wall_rot;
    unsigned int time_entered; /* This only works for 1193-ish hours */
} RhrLocals;

void rhr_reset(RhrLocals* rhr);
void rhr_step(RhrLocals* rhr, Sensors* sens);

#endif
