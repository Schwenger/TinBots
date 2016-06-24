/*
 * Right-Hand Follower (Right Hand Rule)
 */

#include <assert.h>

#include "blind-cop.h"

static const hal_time NO_PATH_TIMEOUT = 40;

enum BLIND_STATES {
    BLIND_init_rhr,
    BLIND_follow_path,
    BLIND_pre_no_path,
    BLIND_no_path,
    BLIND_party
};
/* static_assert(sizeof(enum BLIND_STATES) == sizeof(BlindLocals::state_big),
 *               "Bad things"); */
typedef char check_blind_state_big_size[
    (sizeof(enum BLIND_STATES) == sizeof(int)) ? 1 : -1];

enum BLIND_LEAF_STATES {
    /* BLIND_init_rhr */
    BLIND_LEAF_init_rhr,
    BLIND_LEAF_sense_angle,
    BLIND_LEAF_wait_angle_zero,
    /* BLIND_follow_path */
    BLIND_LEAF_run_to_victim,
    BLIND_LEAF_wait_path_zero,
        /* No 'docking' state (yet?) */
    BLIND_LEAF_outta_here
    /* BLIND_no_path */
        /* No sub states here */
    /* BLIND_party */
        /* No sub states here */
};
/* static_assert(sizeof(enum BLIND_LEAF_STATES) == sizeof(BlindLocals::state_leaf),
 *               "Bad things"); */
typedef char check_blind_state_leaf_size[
    (sizeof(enum BLIND_LEAF_STATES) == sizeof(int)) ? 1 : -1];

static int time_passed_p(const hal_time entered, const hal_time wait_secs) {
    /* TODO: Candidate for code duplication (similar function alread in RHR) */
    const hal_time now = hal_get_time();
    const hal_time wait_ticks = wait_secs * 1000;
    /* If this assert fails, you only need to fix this part.
     * Note that it won't fail for roughly 1193 hours (see e_time.h) */
    assert(now >= entered);
    return entered - now >= wait_ticks;
}

void blind_reset(BlindState* blind) {
    blind->locals.state_big = BLIND_init_rhr;
    blind->locals.state_leaf = BLIND_LEAF_init_rhr;
    /* No further initialization needed because BLIND_init_rhr doesn't
     * read from blind->time_entered */
    blind->run_choice = BLIND_RUN_CHOICE_none;
    /* No further initialization needed because dst_{x,y} should only
     * be read for path_finder */
}

static void switch_failed(BlindState* blind, const char* msg) {
    /* Uhh */
    hal_print(msg);
    assert(0);
    blind_reset(blind);
}

void blind_step(BlindInputs* inputs, BlindState* blind) {
    const int old_state = blind->locals.state_big;
    switch (blind->locals.state_big) {
    case BLIND_init_rhr:
        if (inputs->found_victim_xy) {
            blind->locals.state_big = BLIND_follow_path;
            blind->locals.state_leaf = BLIND_LEAF_run_to_victim;
            blind->run_choice = BLIND_RUN_CHOICE_none;
            blind->dst_x = inputs->victim_x;
            blind->dst_y = inputs->victim_y;
        } else {
            switch (blind->locals.state_leaf) {
            case BLIND_LEAF_init_rhr:
                blind->run_choice = BLIND_RUN_CHOICE_rhr;
                if (inputs->need_angle) {
                    blind->locals.state_leaf = BLIND_LEAF_sense_angle;
                    blind->run_choice = BLIND_RUN_CHOICE_victim_finder;
                }
                break;
            case BLIND_LEAF_sense_angle:
                if (inputs->found_victim_phi) {
                    blind->locals.state_leaf = BLIND_LEAF_wait_angle_zero;
                    blind->run_choice = BLIND_RUN_CHOICE_none;
                }
                break;
            case BLIND_LEAF_wait_angle_zero:
                if (!inputs->found_victim_phi) {
                    blind->locals.state_leaf = BLIND_LEAF_init_rhr;
                    blind->run_choice = BLIND_RUN_CHOICE_rhr;
                }
                break;
            default:
                switch_failed(blind, "BLIND/init_rhr illegal state?!  Resetting ...");
                break;
            }
        }
        break;
    case BLIND_follow_path:
        if (inputs->no_path) {
            blind->locals.state_big = BLIND_pre_no_path;
            /* From here on, state_leaf must stay intact, so we can come back! */
            blind->run_choice = BLIND_RUN_CHOICE_none;
        } else {
            switch (blind->locals.state_leaf) {
            case BLIND_LEAF_run_to_victim:
                /* Might be coming back from no_path */
                blind->run_choice = BLIND_RUN_CHOICE_path_finder;
                if (inputs->path_completed) {
                    blind->locals.state_leaf = BLIND_LEAF_wait_path_zero;
                    blind->run_choice = BLIND_RUN_CHOICE_none;
                }
                break;
            case BLIND_LEAF_wait_path_zero:
                if (!inputs->path_completed) {
                    blind->locals.state_leaf = BLIND_LEAF_outta_here;
                }
                break;
            case BLIND_LEAF_outta_here:
                blind->run_choice = BLIND_RUN_CHOICE_path_finder;
                blind->dst_x = inputs->origin_x;
                blind->dst_y = inputs->origin_y;
                if (inputs->path_completed) {
                    blind->locals.state_big = BLIND_party;
                    blind->run_choice = BLIND_RUN_CHOICE_none;
                }
                break;
            default:
                switch_failed(blind, "BLIND/follow_path illegal state?!  Resetting ...");
                break;
            }
        }
        break;
    case BLIND_pre_no_path:
        /* Just wait until the path_finder has reset. */
        if (!inputs->no_path) {
            blind->locals.state_big = BLIND_no_path;
            blind->run_choice = BLIND_RUN_CHOICE_rhr;
        }
        break;
    case BLIND_no_path:
        if (time_passed_p(blind->locals.time_entered, NO_PATH_TIMEOUT)) {
            blind->locals.state_big = BLIND_follow_path;
            /* state_leaf should still be valid.
             * Since we don't know which driver to run, go for 'none',
             * and set it in the appropriate leaf state. */
            blind->run_choice = BLIND_RUN_CHOICE_none;
        }
        break;
    case BLIND_party:
        /* Nothing to do here. */
        break;
    default:
        switch_failed(blind, "blind illegal state?!  Resetting ...");
        break;
    }

    /* Only read by BLIND_no_path */
    if (blind->locals.state_big != old_state) {
        blind->locals.time_entered = hal_get_time();
    }
}
