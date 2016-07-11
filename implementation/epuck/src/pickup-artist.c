#include "state-machine-common.h"
#include "pickup-artist.h"

#define PA_THRESHOLD 10.0

enum {
    PA_INACTIVE,
    PA_SEARCHING,
    PA_DEAD
};

void pa_reset(PickupState* ps) {
    ps->locals.state = PA_INACTIVE;
    ps->locals.time_entered = 0;
    ps->is_dead = 0;
}

void pa_step(PickupState* pa) {
    switch (pa->locals.state) {
    case PA_INACTIVE:
        pa->locals.state = PA_SEARCHING;
        pa->locals.time_entered = hal_get_time();
        smc_move_back();
        break;
    case PA_SEARCHING:
        if (smc_time_passed_p(pa->locals.time_entered, PA_THRESHOLD)) {
            hal_print("[PA]: Giving up!");
            /* Harakiri!! */
            pa->is_dead = 1;
            pa->locals.state = PA_DEAD;
            /* totally_dead(); */
            smc_halt();
        }
        break;
    case PA_DEAD:
        /* Nothing to do here. */
        break;
    default:
        /* Ehhh it's probably alright. */
        break;
    }
}
