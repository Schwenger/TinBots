#ifndef EPUCK_T2T_H
#define EPUCK_T2T_H

#include "hal.h"
#include "map.h"
/* #include "tinbot.h" */
/* Don't include it to resolve the inclusion cycle. */
struct TinBot;

#define T2T_HEARTBEAT_PERIOD_SECS 5

/* All of the following function are intended to be run from withing an ISR.
 * Please make sure they are only called when the package is from the
 * right source! */

/* 'Our' packages: MAY (don't care)
 * 'Their' packages: MUST */
void t2t_receive_heartbeat(struct TinBot* bot);

/* 'Our' packages: MUST NOT
 * 'Their' packages: MUST */
void t2t_receive_found_phi(struct TinBot* bot, double x, double y, double phi);

/* Response of RPi */
void t2t_receive_phi_correction(struct TinBot* bot, double phi_correct, unsigned int acceptable);

/* 'Our' packages: MUST
 * 'Their' packages: MUST
 * Please set the 'is_ours' flag. */
void t2t_receive_found_xy(struct TinBot* bot, int is_ours, int x, int y, int iteration);

/* 'Our' packages: MUST
 * 'Their' packages: MUST */
void t2t_receive_update_map(struct TinBot* bot, int x, int y, Map* map);

/* 'Our' packages: MUST NOT
 * 'Their' packages: MUST */
void t2t_receive_docked(struct TinBot* bot);

/* 'Our' packages: MAY (don't care)
 * 'Their' packages: MAY (don't care)
 * Actually, I don't see any reasonable way we could
 * ever *receive* that command. */
void t2t_receive_completed(struct TinBot* bot);

typedef struct T2TData {
    /* Flags */
    hal_time newest_beat;
    unsigned char own_first_p;
    unsigned char someone_docked_p;
    unsigned char someone_completed_p;
    /* Counters */
    unsigned char newest_own;
    unsigned char newest_theirs;
} T2TData;

/* Should be called once in each main loop iteration:
 * - matlab: in the stateflow chart
 * - tests: if needed by the test
 * - epuck: main.c */
void t2t_pump(struct TinBot* bot);

#endif
