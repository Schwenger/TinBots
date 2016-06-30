#include <assert.h>
#include <stdlib.h>

#include "hal/hal.h"
#include "pi.h"
#include "sensors.h"
#include "state-machine-common.h"
#include "victim-direction.h"

/* Invariants:
 * state==VD_done \iff precisely one output flag is set
 * state!=VD_done \iff none of the output flags are set */
enum {
    VD_off,
    VD_start,
    VD_found_start,
    VD_rotate_end,
    VD_done
};

static void entry_start(VDState* vd, Sensors* sens);
static double determine_victim_phi(double bound, double current_phi, int sensor);

static const hal_time vd_turn_ticks =
    (hal_time)(1000 * 2 * M_PI / SMC_ROT_PER_SEC);

void vd_reset(VDState* vd){
    vd->state = VD_off;
    vd->victim_found = 0;
    vd->victim_phi = 0;
    vd->give_up = 0;
}

static void entry_start(VDState* vd, Sensors* sens) {
    int i;
    vd->state = VD_start;
    vd->locals.sensor_id = -1;
    for(i = 0; i < 6; ++i) {
        if(sens->ir[i] == 0){
            vd->locals.sensor_id = i;
        }
    }
    if (vd->locals.sensor_id == -1) {
        vd->state = VD_done;
        vd->give_up = 1;
    } else {
        assert(sens->ir[vd->locals.sensor_id] == 0);
        smc_rot_right();
        vd->locals.time_begin = hal_get_time();
    }
}

static double determine_victim_phi(double ir_start, double ir_end, int sensor) {
    double look_phi, victim_phi;
    assert(ir_end < ir_start);
    assert(ir_end > ir_start - 2 * M_PI);

    look_phi = (ir_start + ir_end) / 2;
    victim_phi = look_phi + (sensor - 1) * M_PI/3;
    victim_phi = fmod(victim_phi, 2 * M_PI);

    return victim_phi;
}

void vd_step(VDState* vd, Sensors* sens){
    switch(vd->state) {
        case VD_off:
            entry_start(vd, sens);
            break;
        case VD_start:
            if(sens->ir[vd->locals.sensor_id]){
                vd->locals.time_ir_on = hal_get_time();
                vd->state = VD_found_start;
            } else if (hal_get_time() - vd->locals.time_begin >= vd_turn_ticks) {
                vd->give_up = 1;
                vd->state = VD_done;
                smc_halt();
            }
            break;
        case VD_found_start:
            if(sens->ir[vd->locals.sensor_id] == 0){
                const hal_time time_ir_off = hal_get_time();
                const double phi_off = sens->current.direction;
                const double phi_on = phi_off -
                    (time_ir_off - vd->locals.time_ir_on) * 1000 * SMC_ROT_PER_SEC;
                vd->victim_phi = determine_victim_phi(phi_on, phi_off, vd->locals.sensor_id);
                vd->state = VD_rotate_end;
            } else if (hal_get_time() - vd->locals.time_begin >= vd_turn_ticks) {
                vd->give_up = 1;
                vd->state = VD_done;
                smc_halt();
            }
            break;
        case VD_rotate_end:
            if (hal_get_time() - vd->locals.time_begin >= vd_turn_ticks) {
                vd->victim_found = 1;
                vd->state = VD_done;
                smc_halt();
            }
            break;
        case VD_done:
            break;
        default:
            hal_print("Invalid state in victim direction. VICTOR, where are you??");
            assert(0);
            vd_reset(vd);
            break;
    }
}
