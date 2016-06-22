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
/* Must be a macro, as xc16 is too dumb. */
#define MV_PER_SEC (2.0 /* FIXME ??? */)
/* Must be a macro, as xc16 is too dumb. */
#define ROT_PER_SEC (MV_PER_SEC * 2 / 5.3)

static const double SECS_PER_DEGREE = (M_PI / 180) / ROT_PER_SEC;

static void rhr_rot_left(void) {
    hal_set_speed(-RHR_MOTOR_ROT, RHR_MOTOR_ROT);
}

static void rhr_rot_right(void) {
    hal_set_speed(RHR_MOTOR_ROT, -RHR_MOTOR_ROT);
}

static void rhr_move(void) {
    hal_set_speed(RHR_MOTOR_MV, RHR_MOTOR_MV);
}

static int time_passed_p(const e_time_t entered, const double wait_secs) {
    const e_time_t now = get_e_time();
    const e_time_t wait_ticks = (e_time_t)(wait_secs * E_TICKS_PER_SEC);
    /* If this assert fails, you only need to fix this part.
     * Note that it won't fail for roughly 1193 hours (see e_time.h) */
    assert(now >= entered);
    return entered - now >= wait_ticks;
}

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
            rhr_move();
        } else {
            if (rhr->wall_rot < 0) {
                rhr_rot_right();
                rhr->wall_rot = -rhr->wall_rot;
            } else {
                rhr_rot_left();
            }
            rhr->state = RHR_wall_wait;
        }
        break;
    case RHR_find_wall:
        if (sens->proximity[PROXIMITY_M_20] <= RHR_SENSE_TOL
            || sens->proximity[PROXIMITY_M_45] <= RHR_SENSE_TOL
            || sens->proximity[PROXIMITY_P_20] <= RHR_SENSE_TOL
            || sens->proximity[PROXIMITY_P_45] <= RHR_SENSE_TOL) {
            rhr->state = RHR_check_wall;
            /* Keep going (don't change motors) */
            break;
        }
        break;
    case RHR_wall_wait:
        if (time_passed_p(rhr->time_entered, rhr->wall_rot / ROT_PER_SEC)) {
            rhr->state = RHR_wall_orient;
            rhr_rot_right();
        }
        break;
    case RHR_wall_orient:
        if (sens->proximity[PROXIMITY_M_45] <= RHR_SENSE_TOL) {
            rhr->state = RHR_avoid_coll;
            rhr_rot_left();
        }
        break;
    case RHR_avoid_coll:
        if (sens->proximity[PROXIMITY_M_90] <= RHR_SENSE_TOL) {
            rhr->state = RHR_claustrophobia;
            rhr_rot_left();
        } else if (sens->proximity[PROXIMITY_M_45] <= RHR_SENSE_TOL
                   && time_passed_p(rhr->time_entered, 5 * SECS_PER_DEGREE)) {
            rhr->state = RHR_wall_orient;
            rhr_move();
        }
        break;
    case RHR_claustrophobia:
        if (sens->proximity[PROXIMITY_M_20] > RHR_SENSE_TOL
            && time_passed_p(rhr->time_entered, 5 * SECS_PER_DEGREE)) {
            rhr->state = RHR_follow;
            rhr_move();
        }
        break;
    case RHR_follow:
        if (sens->proximity[PROXIMITY_M_20] <= RHR_SENSE_TOL) {
            rhr->state = RHR_claustrophobia;
            rhr_rot_left();
        } else if (sens->proximity[PROXIMITY_M_45] > RHR_SENSE_TOL) {
            rhr->state = RHR_go_on;
        }
        break;
    case RHR_go_on:
        if (sens->proximity[PROXIMITY_M_20] <= RHR_SENSE_TOL) {
            rhr->state = RHR_claustrophobia;
            rhr_rot_left();
        } else if (sens->proximity[PROXIMITY_M_45] <= RHR_SENSE_TOL) {
            rhr->state = RHR_follow;
            rhr_move();
        } else if (time_passed_p(rhr->time_entered, 10 / MV_PER_SEC)) {
            rhr->state = RHR_stay_close;
            rhr_rot_right();
        }
        break;
    case RHR_stay_close:
        if (time_passed_p(rhr->time_entered, 70 * SECS_PER_DEGREE)) {
            rhr->state = RHR_run_close;
            rhr_move();
        }
        break;
    case RHR_run_close:
        if (sens->proximity[PROXIMITY_M_20] <= RHR_SENSE_TOL) {
            rhr->state = RHR_wall_orient;
            rhr_move();
        } else if (sens->proximity[PROXIMITY_M_45] > RHR_SENSE_TOL
                   && time_passed_p(rhr->time_entered, 10 / MV_PER_SEC)) {
            rhr->state = RHR_stay_close;
            rhr_rot_right();
        }
        break;
    default:
        /* Uhh */
        assert(0);
        hal_print("RHR illegal state?!  Resetting ...");
        rhr->state = RHR_check_wall;
        hal_set_speed(0, 0);
        break;
    }

    if (rhr->state != old_state) {
        rhr->time_entered = get_e_time();
    }
}