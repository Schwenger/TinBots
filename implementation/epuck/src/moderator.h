#ifndef EPUCK_MODERATOR_H
#define EPUCK_MODERATOR_H

#include "hal.h"
#include "t2t-data.h"

typedef struct ModInputs {
    T2TData_Moderate* t2t_data;
    double own_victim_x;
    double own_victim_y;
    unsigned int found_victim_xy;
} ModInputs;

typedef enum ModDecision {
    MOD_DECISION_SEARCHING,
    MOD_DECISION_WAITING,
    MOD_DECISION_RESCUEING,
    MOD_DECISION_DEAD
} ModDecision;

typedef struct ModLocals {
    hal_time time_entered;
    int state;
    int sent_iteration; /* Must be signed */
} ModLocals;

typedef struct ModState {
    ModLocals locals;
    ModDecision decision;
    /* Use the same position on *every* Tin Bot.
     * That's necessary in the following edge case:
     * - we didn't successfully triangulate VICTOR
     * - someone else claims they did
     * - they die
     * - we win the bidding to rescue VICTOR, but don't
     *   know any position of our own.
     * Also, it's nice to know that all Bots are "synchronized". */
    double victim_x;
    double victim_y;
} ModState;

void mod_reset(ModState* mod);
void mod_step(ModInputs* inputs, ModState* mod);

#endif
