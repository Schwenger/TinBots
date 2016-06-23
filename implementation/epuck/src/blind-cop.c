/*
 * Right-Hand Follower (Right Hand Rule)
 */

#include <assert.h>

#include "blind-cop.h"

enum BLIND_STATES {
    BLIND_init_rhr,
    BLIND_follow_path,
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

void blind_reset(BlindLocals* blind) {
    blind->state_big = BLIND_init_rhr;
    blind->state_leaf = BLIND_LEAF_init_rhr;
    /* No further initialization needed because BLIND_init_rhr doesn't
       read from blind->time_entered */
}

static void switch_failed(BlindLocals* blind, const char* msg) {
    /* TODO: Candidate for code duplication (similar function alread in RHR) */
    /* Uhh */
    hal_print(msg);
    assert(0);
    blind_reset(blind);
}

void blind_step(BlindInputs* inputs, BlindLocals* blind, Sensors* sens) {
    const int old_state = blind->state_big;
    switch (blind->state_big) {
    case BLIND_init_rhr:
        switch (blind->state_leaf) {
        case BLIND_LEAF_init_rhr:
            /*FIXME*/
            break;
        case BLIND_LEAF_sense_angle:
            /*FIXME*/
            break;
        case BLIND_LEAF_wait_angle_zero:
            /*FIXME*/
            break;
        default:
            switch_failed(blind, "BLIND/init_rhr illegal state?!  Resetting ...");
            break;
        }
        break;
    case BLIND_follow_path:
        switch (blind->state_leaf) {
        case BLIND_LEAF_run_to_victim:
            /*FIXME*/
            break;
        case BLIND_LEAF_wait_path_zero:
            /*FIXME*/
            break;
        default:
            switch_failed(blind, "BLIND/follow_path illegal state?!  Resetting ...");
            break;
        }
        break;
    case BLIND_no_path:
        /*FIXME*/
        break;
    case BLIND_party:
        /*FIXME*/
        break;
    default:
        switch_failed(blind, "blind illegal state?!  Resetting ...");
        break;
    }

    /* only for BLIND_no_path */
    if (blind->state_big != old_state) {
        blind->time_entered = hal_get_time();
    }
}
