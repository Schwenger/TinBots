/*
 * Right-Hand Follower (Right Hand Rule)
 */

#include <assert.h>

#include "pi.h" /* M_PI */
#include "hal.h"
#include "rhr.h"

#define NUM_PROXIMITY (8)


enum RHR_STATES {
    RHR_check_wall, /* I was tempted to call this "czech wall" */
    RHR_find_wall,
    RHR_wall_wait,
    RHR_wall_orient,
    RHR_avoid_coll,
    RHR_claustrophobia,
    RHR_follow,
    RHR_go_on,
    RHR_stay_close,
    RHR_run_close
};

void rhr_reset(RhrLocals* rhr) {
    rhr->state = RHR_check_wall;
}

static void find_wall(RhrLocals* rhr, Sensors* sens) {
    /* see virt_proto/software/find_wall.m */
    static const double sense_angles[NUM_PROXIMITY] = {
        -150*M_PI/180,
         -90*M_PI/180,
         -45*M_PI/180,
         -20*M_PI/180,
         +20*M_PI/180,
         +45*M_PI/180,
         +90*M_PI/180,
        +150*M_PI/180};
    static const unsigned int order[NUM_PROXIMITY] =
        {4 - 1, 3 - 1, 5 - 1, 2 - 1, 6 - 1, 1 - 1, 7 - 1, 8 - 1};
    /* rot_angles = sense_angles - (-20); */
    static const double threshold = 10;
    unsigned i;

    assert(sizeof(sense_angles) / sizeof(sense_angles[0])
            == NUM_PROXIMITY);
    assert(sizeof(sens->proximity) / sizeof(sens->proximity[0])
            == NUM_PROXIMITY);

    rhr->wall_p = 0;
    for(i = 0; i < 8; ++i) {
        unsigned int s = order[i];
        if (sens->ir[s] <= threshold) {
            rhr->wall_p = 1;
            rhr->wall_rot = sense_angles[s] - (-20.0*M_PI/180.0);
            return;
        }
    }
}

void rhr_step(RhrLocals* rhr, Sensors* sens) {
    const int old_state = rhr->state;
    switch (rhr->state) {
    case RHR_check_wall:
        /* Don't change motor state, in case we're coming from find_wall */
        find_wall(rhr, sens);
        if (!rhr->wall_p) {
            rhr->state = RHR_find_wall;
        } else {
            if (rhr->wall_rot < 0) {
                /* Rotate right */
                set_speed(1, -1);
                rhr->wall_rot = -rhr->wall_rot;
            } else {
                /* Rotate left */
                set_speed(1, -1);
            }
            rhr->state = RHR_wall_wait;
        }
        break;
    case RHR_find_wall:
        /* FIXME */
        break;
    case RHR_wall_wait:
        /* FIXME */
        break;
    case RHR_wall_orient:
        /* FIXME */
        break;
    case RHR_avoid_coll:
        /* FIXME */
        break;
    case RHR_claustrophobia:
        /* FIXME */
        break;
    case RHR_follow:
        /* FIXME */
        break;
    case RHR_go_on:
        /* FIXME */
        break;
    case RHR_stay_close:
        /* FIXME */
        break;
    case RHR_run_close:
        /* FIXME */
        break;
    default:
        /* Uhh */
        assert(0);
        print("RHR illegal state?!  Resetting ...");
        rhr->state = RHR_check_wall;
        break;
    }

    if (rhr->state != old_state) {
        /* rhr->time_entered = get_time(); */
        rhr->time_entered = 42; /* FIXME */
    }
}
