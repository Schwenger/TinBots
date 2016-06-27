/*
 * Right-Hand Follower (Right Hand Rule)
 */

#include <assert.h>
#include <math.h>

#include "pi.h" /* M_PI */
#include "path-exec.h"

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
static const double PE_MOTOR_MV = 2;
static const double PE_MOTOR_ROT = 1;

/* Must be a macro, as xc16 is too dumb. */
#define MV_PER_SEC (2.0 /* FIXME ??? */)
/* Must be a macro, as xc16 is too dumb. */
#define ROT_PER_SEC (MV_PER_SEC * 2 / 5.3)

static void pe_rot_left(void) {
    /* TODO: Candidate for code deduplication */
    hal_set_speed(-PE_MOTOR_ROT, PE_MOTOR_ROT);
}

static void pe_rot_right(void) {
    /* TODO: Candidate for code deduplication */
    hal_set_speed(PE_MOTOR_ROT, -PE_MOTOR_ROT);
}

static void pe_move(void) {
    /* TODO: Candidate for code deduplication */
    hal_set_speed(PE_MOTOR_MV, PE_MOTOR_MV);
}

static void pe_halt(void) {
    hal_set_speed(0, 0);
}

static int time_passed_p(const hal_time entered, const double wait_secs) {
    /* TODO: Candidate for code deduplication */
    const hal_time now = hal_get_time();
    const hal_time wait_ticks = (hal_time)(wait_secs * 1000);
    /* If this assert fails, you only need to fix this part.
     * Note that it won't fail for roughly 1193 hours (see e_time.h) */
    assert(now >= entered);
    return entered - now >= wait_ticks;
}

void pe_reset(PathExecState* pe) {
    pe->done = 0;
    pe->locals.state = PE_inactive;
    /* No further initialization needed because PE_inactive doesn't
       read from locals.time_entered or locals.start_* or locals.need_* */
}

void pe_step(PathExecInputs* inputs, PathExecState* pe, Sensors* sens) {
    const int old_state = pe->locals.state;

    if (!inputs->drive_p) {
        /* We just got notified that we shouldn't be running anymore. */
        pe->locals.state = PE_inactive;
        /* It's okay to 'halt()' many times per second. */
        pe_halt();
        /* Don't set 'time_entered'; not needed. */
        return;
    }

    switch (pe->locals.state) {
    case PE_inactive:
        if(inputs->drive_p) {
            pe->locals.state = PE_compute;
        }
        break;
    case PE_compute:
        {
            double start_dir;
            double target_dir;
            pe->locals.start_x = sens->current.x;
            pe->locals.start_y = sens->current.y;
            start_dir = sens->current.direction;
            target_dir = atan2(inputs->next_y - pe->locals.start_y,
                               inputs->next_x - pe->locals.start_x);
            pe->locals.need_rot = fmod(target_dir - start_dir + M_PI,
                                       2 * M_PI) - M_PI;
            pe->locals.need_rot /= ROT_PER_SEC;
            pe->locals.normal_x = -(inputs->next_y - pe->locals.start_y);
            pe->locals.normal_y =   inputs->next_x - pe->locals.start_x;
            pe->locals.need_dist = pe->locals.normal_x * pe->locals.normal_x
                                 + pe->locals.normal_y * pe->locals.normal_y;
            pe->locals.need_dist = sqrt(pe->locals.need_dist);
            pe->locals.normal_x /= pe->locals.need_dist;
            pe->locals.normal_y /= pe->locals.need_dist;
            pe->locals.need_dist /= MV_PER_SEC;

            /* Code from the transitions */
            pe->locals.state = PE_rotate;
            if (pe->locals.need_rot < 0) {
                pe->locals.need_rot *= -1;
                pe_rot_right();
            } else {
                pe_rot_left();
            }
        }
        break;
    case PE_rotate:
        if (time_passed_p(pe->locals.time_entered, pe->locals.need_rot)) {
            pe->locals.state = PE_drive;
            pe_move();
        }
        break;
    case PE_drive:
        {
            double stray;
            stray = 0;
            stray += (inputs->next_x - sens->current.x) * pe->locals.normal_x;
            stray += (inputs->next_y - sens->current.y) * pe->locals.normal_y;
            stray = fabs(stray);
            if (stray >= PE_MAX_STRAY) {
                /* Whoopsie daisy. */
                pe->locals.state = PE_compute;
                pe_halt();
            }
        }
        if (time_passed_p(pe->locals.time_entered, pe->locals.need_dist)) {
            pe->locals.state = PE_profit;
            pe_halt();
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
        pe_halt();
        break;
    }

    if (pe->locals.state != old_state) {
        pe->locals.time_entered = hal_get_time();
    }
}
