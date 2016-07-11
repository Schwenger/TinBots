#ifndef EPUCK_PICKUP_ARTIST_H
#define EPUCK_PICKUP_ARTIST_H

#include "hal.h"
#include "sensors.h"

typedef struct PickupLocals {
    int state;
    hal_time time_entered;
} PickupLocals;

typedef struct PickupState {
    PickupLocals locals;
    int is_dead;
} PickupState;

/* Finally, it's not PulseAudio */
void pa_reset(PickupState* ps);
void pa_step(PickupState* ps);

#endif
