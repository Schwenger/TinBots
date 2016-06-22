#ifndef RHR_H
#define RHR_H

#include "e_time.h"
#include "sensors.h"

typedef struct RhrLocals {
    int state; /* The state we will *enter* at the next step() */
    int wall_p;
    double wall_rot;
    e_time_t time_entered;
} RhrLocals;

void rhr_reset(RhrLocals* rhr);
void rhr_step(RhrLocals* rhr, Sensors* sens);

#endif
