#include <assert.h>
#include <stdlib.h>

#include "hal/hal.h"
#include "pi.h"
#include "sensors.h"
#include "victim-direction.h"

enum {
    VD_off,
    VD_start,
    VD_found_end,
    VD_found_start,
    VD_done
};

static void entry_start(VDState* vd, Sensors* sens);
static double determine_victim_phi(double bound, double current_phi, int sensor);

void vd_reset(VDState* vd){
    vd->state = VD_off;
    vd->victim_found = 0;
}

static void entry_start(VDState* vd, Sensors* sens) {
    int i;
    vd->state = VD_start;
    vd->victim_found = 0;
    vd->locals.sensor = -1;
    for(i = 0; i < 6; ++i) {
        if(sens->ir[i] == 0){
            vd->locals.sensor = i;
        }
    }
    assert(sens->ir[vd->locals.sensor] == 0);
    hal_set_speed(-1, 1);
    vd->locals.initial_phi = sens->current.direction;
}

static double determine_victim_phi(double ir_start, double ir_end, int sensor) {
    double look_phi, victim_phi;
    if(ir_end < ir_start) {
        ir_end += 2 * M_PI;
    }
    if(ir_end < ir_start){ /* adds an extra layer of fail safety. */
        hal_print("Crazy corner case!!! Ben was right!");
        ir_end += 2 * M_PI;
    }
    assert(ir_end < ir_start);

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
            if(sens->ir[vd->locals.sensor]){
                vd->locals.bound1 = sens->current.direction;
                vd->state = VD_found_start;
            }
            break;
        case VD_found_start:
            if(sens->ir[vd->locals.sensor] == 0){
                vd->victim_phi = determine_victim_phi(vd->locals.bound1, sens->current.direction, vd->locals.sensor);
                vd->state = VD_found_end;
            }
            break;
        case VD_found_end:
            if(fabs(sens->current.direction - vd->locals.initial_phi) < 4 * M_PI/180.0){
                vd->state = VD_done;
                hal_set_speed(0, 0);
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
