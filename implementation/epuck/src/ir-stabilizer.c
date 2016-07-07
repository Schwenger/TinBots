#include <assert.h>
#include <hal/hal.h>
#include "ir-stabilizer.h"

enum {
    IRS_NOTHING,
    IRS_SOMETHING,
    IRS_TEDDY
};

void irs_reset(IRSState* irs) {
    irs->locals.state = IRS_NOTHING;
    irs->ir_stable = 0;
}

void irs_step(IRSState* irs, Sensors* sens) {
    hal_time now;
    int disjuctive_ir, i;
    disjuctive_ir = 0;
    for(i = 0; i < 6; ++i)
        disjuctive_ir += sens->ir[i];
    switch(irs->locals.state) {
        case IRS_NOTHING:
            if(disjuctive_ir) {
                irs->locals.state = IRS_SOMETHING;
                irs->locals.entry = hal_get_time();
            }
            break;
        case IRS_SOMETHING:
            now = hal_get_time();
            if(!disjuctive_ir) {
                irs->locals.state = IRS_NOTHING;
            } else if(now - irs->locals.entry >= 1000) {
                irs->locals.state = IRS_TEDDY;
                irs->ir_stable = 1;
            }
            break;
        case IRS_TEDDY:
            if(!disjuctive_ir) {
                irs->locals.state = IRS_NOTHING;
                irs->ir_stable = 0;
            }
            break;
        default:
            /* Uhh */
            hal_print("IR-Stabilizer illegal state?!  Resetting ...");
            assert(0);
            irs_reset(irs);
            break;
    }
}
