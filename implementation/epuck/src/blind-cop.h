#ifndef BLIND_COP_H
#define BLIND_COP_H

#include "hal.h"
#include "sensors.h"

typedef struct BlindInputs {
    int found_victim_phi;
    int found_victim_xy;
    int need_angle;
    int no_path;
    double origin_x;
    double origin_y;
    double victim_x;
    double victim_y;
} BlindInputs;

typedef struct BlindLocals {
    int state_big;
    int state_leaf; /* or history of BLIND_follow_path */
    hal_time time_entered; /* only for BLIND_no_path */
} BlindLocals;

enum BlindRunChoice {
    BLIND_RUN_CHOICE_path_finder,
    BLIND_RUN_CHOICE_rhr,
    BLIND_RUN_CHOICE_victim_finder
};

typedef struct BlindOutputs {
    double dst_x;
    double dst_y;
    enum BlindRunChoice run_choice;
} BlindOutputs;

void blind_reset(BlindLocals* rhr);
void blind_step(BlindInputs* inputs, BlindLocals* rhr, Sensors* sens);

#endif
