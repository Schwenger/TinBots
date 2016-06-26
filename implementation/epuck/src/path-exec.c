/*
 * Right-Hand Follower (Right Hand Rule)
 */

#include <assert.h>

#include "pi.h" /* M_PI */
#include "path-exec.h"
#include "map.h"

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

static const double epsilon = 0.001;

/* Must be a macro, as xc16 is too dumb. */
#define MV_PER_SEC (2.0 /* FIXME ??? */)
/* Must be a macro, as xc16 is too dumb. */
#define ROT_PER_SEC (MV_PER_SEC * 2 / 5.3)

static const double SECS_PER_DEGREE = (M_PI / 180) / ROT_PER_SEC;

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
    Position pos, dest;
    double delta_phi;
    const int old_state = pe->locals.state;

    switch (pe->locals.state) {
    case PE_inactive:
        if(inputs->drive_p)
            pe->locals.state = PE_rotate;
        break;
    case PE_compute:
        /* FIXME */
        break;
    case PE_rotate:
        delta_phi = sens->current.direction - pe->locals.desired_angle;
        if(fabs(delta_phi) < epsilon){
            pe->locals.state = PE_drive;
            pe_halt();
        } else {
            delta_phi > 0 ? pe_rot_right() : pe_rot_left();
        }
        break;
    case PE_drive:

        pos = map_discretize(sens->current.x, sens->current.y);
        dest = map_discretize(inputs->next_x, inputs->next_y);
        /* TODO include mechanism checking for problems with angle, i.e. moving past the goal */
        if(pos.x == dest.x && pos.y == dest.y){
            pe->locals.state = PE_profit; /* not sure about the semantics of profit */
            pe_halt();
        } else {
            pe_move();
        }
        break;
    case PE_profit:
        /* FIXME */
        break;
    default:
        /* Uhh */
        hal_print("PathExec illegal state?!  Resetting ...");
        assert(0);
        pe_reset(pe);
        hal_set_speed(0, 0);
        break;
    }

    if (pe->locals.state != old_state) {
        pe->locals.time_entered = hal_get_time();
    }
}
