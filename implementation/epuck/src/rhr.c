/*
 * Right-Hand Follower (Right Hand Rule)
 */

#include <assert.h>

#include "pi.h" /* M_PI */
#include "hal.h"
#include "rhr.h"


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

static const double RHR_SENSE_TOL = 10;
static const double RHR_MOTOR_MV = 2;
static const double RHR_MOTOR_ROT = 1;
static const double MV_PER_SEC = 2; /* FIXME ??? */
static const double ROT_PER_SEC = MV_PER_SEC * 2 / 5.3;

void rhr_reset(RhrLocals* rhr) {
    rhr->state = RHR_check_wall;
    /* No further initialization needed because RHR_check_wall doesn't
       read from rhr->time_entered */
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
        {PROXIMITY_M_20, PROXIMITY_M_45, PROXIMITY_P_20, PROXIMITY_M_90,
         PROXIMITY_P_45, PROXIMITY_M_150, PROXIMITY_P_90, PROXIMITY_P_150};
    /* rot_angles = sense_angles - (-20); */
    unsigned i;

    assert(sizeof(sense_angles) / sizeof(sense_angles[0])
            == NUM_PROXIMITY);
    assert(sizeof(sens->proximity) / sizeof(sens->proximity[0])
            == NUM_PROXIMITY);

    rhr->wall_p = 0;
    for(i = 0; i < 8; ++i) {
        unsigned int s = order[i];
        if (sens->ir[s] <= RHR_SENSE_TOL) {
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
        find_wall(rhr, sens);
        if (!rhr->wall_p) {
            rhr->state = RHR_find_wall;
            /* Move forward */
            set_speed(RHR_MOTOR_MV, RHR_MOTOR_MV);
        } else {
            if (rhr->wall_rot < 0) {
                /* Rotate right */
                set_speed(RHR_MOTOR_ROT, -RHR_MOTOR_ROT);
                rhr->wall_rot = -rhr->wall_rot;
            } else {
                /* Rotate left */
                set_speed(-RHR_MOTOR_ROT, RHR_MOTOR_ROT);
            }
            rhr->state = RHR_wall_wait;
        }
        break;
    case RHR_find_wall:
        /* -45, -20, +20, +45 degree are the sensors 2, 3, 4, 5 */
        {
            unsigned int i;
            for (i = 2; i < 6; ++i) {
                if (sens->proximity[i] <= RHR_SENSE_TOL) {
                    rhr->state = RHR_check_wall;
                    /* Keep going (don't change motors) */
                    break;
                }
            }
        }
        break;
    case RHR_wall_wait:
        {
            const double wait_sec = rhr->wall_rot / ROT_PER_SEC;
            const e_time_t wait_ticks = (e_time_t)(wait_sec * E_TICKS_PER_SEC);
            const e_time_t end_time = rhr->time_entered + wait_ticks;
            assert(end_time >= rhr->time_entered);
            if (get_e_time() >= end_time) {
                rhr->state = RHR_wall_orient;
            }
        }
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
        rhr->time_entered = get_e_time();
    }
}
