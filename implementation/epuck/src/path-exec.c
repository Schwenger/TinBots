/*
 * Right-Hand Follower (Right Hand Rule)
 */

#include <assert.h>
#include <math.h>

#include "pi.h" /* M_PI */
#include "path-exec.h"
#include "state-machine-common.h"

enum PE_STATES {
    PE_inactive,
    PE_compute,
    PE_rotate, /* turn to */
    PE_drive, /* tune in */
    PE_profit /* and drop out */
};
/* static_assert(sizeof(enum PE_STATES) == sizeof(PathExecLocals::state),
 *               "Bad things"); */
typedef char check_pe_states_size[
    (sizeof(enum PE_STATES) == sizeof(int)) ? 1 : -1];

static const double PE_MAX_STRAY = 10;

void pe_reset(PathExecState* pe) {
    pe->done = 0;
    pe->see_obstacle = 0;
    pe->locals.state = PE_inactive;
    /* No further initialization needed because PE_inactive doesn't
       read from locals.time_entered or locals.start_* or locals.need_* */
}

void pe_step(PathExecInputs* inputs, PathExecState* pe, Sensors* sens) {
    PathExecLocals* l;
    int old_state;

    l = &pe->locals;
    old_state = l->state;

    if (!inputs->drive) {
        /* We just got notified that we shouldn't be running anymore. */
        l->state = PE_inactive;
        /* It's okay to 'halt()' many times per second. */
        smc_halt();
        /* Don't set 'time_entered'; not needed. */
        return;
    }

    switch (l->state) {
    case PE_inactive:
        if(inputs->drive) {
            l->state = PE_compute;
        }
        break;
    case PE_compute:
        {
            double start_dir;
            double target_dir;
            l->start_x = sens->current.x;
            l->start_y = sens->current.y;
            start_dir = sens->current.direction;
            target_dir = atan2(inputs->next_y - l->start_y,
                               inputs->next_x - l->start_x);
            if(inputs->backwards) {
                target_dir += M_PI;
            }
            l->need_rot = fmod(target_dir - start_dir + M_PI, 2 * M_PI) - M_PI;
            l->need_rot /= SMC_ROT_PER_SEC;
            l->normal_x = -(inputs->next_y - l->start_y);
            l->normal_y =   inputs->next_x - l->start_x;
            l->need_dist = l->normal_x * l->normal_x + l->normal_y * l->normal_y;
            l->need_dist = sqrt(l->need_dist);
            l->normal_x /= l->need_dist;
            l->normal_y /= l->need_dist;
            l->need_dist /= SMC_MV_PER_SEC;

            /* Code from the transitions */
            l->state = PE_rotate;
            if (l->need_rot < 0) {
                l->need_rot *= -1;
                smc_rot_right();
            } else {
                smc_rot_left();
            }
        }
        break;
    case PE_rotate:
        if (smc_time_passed_p(l->time_entered, l->need_rot)) {
            l->state = PE_drive;
            if (inputs->backwards) {
                smc_move();
            } else {
                smc_move_back();
            }
        }
        break;
    case PE_drive:
        if (sens->proximity[PROXIMITY_M_20] < 1
            || sens->proximity[PROXIMITY_P_20] < 1) {
            smc_halt();
            l->state = PE_profit;
            pe->see_obstacle = 1;
            break;
        }
        {
            double stray;
            stray = 0;
            stray += (inputs->next_x - sens->current.x) * l->normal_x;
            stray += (inputs->next_y - sens->current.y) * l->normal_y;
            stray = fabs(stray);
            if (stray >= PE_MAX_STRAY) {
                /* Whoopsie daisy. */
                l->state = PE_compute;
                smc_halt();
                break;
            }
        }
        if (smc_time_passed_p(l->time_entered, l->need_dist)) {
            l->state = PE_profit;
            smc_halt();
            pe->done = 1;
        }
        break;
    case PE_profit:
        /* Nothing to do here. */
        break;
    default:
        /* Uhh */
        hal_print("PathExec illegal state?!  Resetting ...");
        assert(0);
        pe_reset(pe);
        smc_halt();
        break;
    }

    if (l->state != old_state) {
        l->time_entered = hal_get_time();
    }
}
