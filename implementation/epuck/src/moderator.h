#ifndef EPUCK_MODERATOR_H
#define EPUCK_MODERATOR_H

#include "hal.h"

typedef enum ModDecision {
    MOD_DECISION_SEARCHING,
    MOD_DECISION_WAITING,
    MOD_DECISION_RESCUEING,
    MUD_DECISION_DEAD
} ModDecision;

typedef struct ModLocals {
    hal_time time_entered;
    int state;
    unsigned char iteration;
    volatile char seen_beat_p;
    volatile unsigned char seen_newest_own;
    volatile unsigned char seen_newest_theirs;
    volatile char seen_own_first_p;
} ModLocals;

typedef struct ModState {
    ModLocals locals;
    ModDecision decision;
} ModState;

void mod_reset(ModState* mod);
void mod_step(int found_victim_xy, ModState* mod);

#endif
