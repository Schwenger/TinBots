#ifndef BLIND_COP_H
#define BLIND_COP_H

#include "hal.h"

typedef struct BlindInputs {
    int found_victim_xy;
    int need_angle;
    int no_path;
    double origin_x;
    double origin_y;
    double victim_x;
    double victim_y;
    int path_completed;
    int victim_attached;
} BlindInputs;

typedef struct BlindLocals {
    int state_big;
    int state_leaf; /* or history of BLIND_follow_path */
    hal_time time_entered; /* only for BLIND_no_path */
} BlindLocals;

enum BlindRunChoice {
    BLIND_RUN_CHOICE_none,
    BLIND_RUN_CHOICE_rhr,
    BLIND_RUN_CHOICE_victim_finder,
    BLIND_RUN_CHOICE_pickup_artist,
    BLIND_RUN_CHOICE_path_finder
};

typedef struct BlindState {
    BlindLocals locals;
    double dst_x;
    double dst_y;
    enum BlindRunChoice run_choice;
    int is_victim;
} BlindState;

void blind_reset(BlindState* blind);
void blind_step(BlindInputs* inputs, BlindState* blind);

#endif
