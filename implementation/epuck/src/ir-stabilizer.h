#ifndef EPUCK_IR_STABILIZER_H
#define EPUCK_IR_STABILIZER_H

#include "sensors.h"

typedef struct IRSLocals {
    hal_time entry;
    int state;
} IRSLocals;

typedef struct IRSState {
    IRSLocals locals;
    int ir_stable;
} IRSState;

void irs_reset(IRSState* irs);
void irs_step(IRSState* irs, Sensors* sens);

#endif /* EPUCK_IR_STABILIZER_H */
