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
    VD_running,
    VD_done
};

/* We know that the IR sensors should at *least*
 * have a sensibility of this many degrees. */
static const double VD_MIN_ON = 9.0 / 360.0;

static void entry_start(VDState* vd, Sensors* sens);
static double determine_victim_phi(double bound, double current_phi, int sensor);

void vd_reset(VDState* vd){
    vd->locals.state = VD_off;
    vd->victim_found = 0;
    vd->victim_phi = 0;
    vd->give_up = 0;
    vd->locals.counter_total = 1;
    vd->locals.counter_on = 0;
    vd->locals.weighted_sum = 0;
}

static void entry_start(VDState* vd, Sensors* sens) {
    int i;
    vd->locals.state = VD_running;
    vd->locals.sensor_id = -1;
    for(i = 0; i < 6; ++i) {
        if(sens->ir[i] == 0){
            vd->locals.sensor_id = i;
        }
    }
    if (vd->locals.sensor_id == -1) {
        vd->locals.state = VD_done;
        vd->give_up = 1;
    } else {
        assert(sens->ir[vd->locals.sensor_id] == 0);
        hal_debug_out(DEBUG_CAT_VD_IR_ID, vd->locals.sensor_id);
        smc_rot_left();
        vd->locals.counter_on = 1;
        vd->locals.weighted_sum = M_PI;
        vd->locals.time_begin = hal_get_time();
    }
}

static double determine_victim_phi(double ir_start, double ir_end, int sensor) {
    double look_phi, victim_phi;
    assert(ir_end > ir_start);
    assert(ir_end < ir_start + 2 * M_PI);

    look_phi = (ir_start + ir_end) / 2;
    /* TODO: Adapt to real-life angles */
    victim_phi = look_phi + ir_sensor_angle[sensor];
    victim_phi = fmod(victim_phi, 2 * M_PI);

    return victim_phi;
}

static void compute_result(VDState* vd, Sensors* sens) {
    double eff_angle;
    double eff_opening;

    if (vd->locals.counter_total < 100) {
        /* Wat. */
        vd->give_up = 1;
        return;
    }

    eff_opening = vd->locals.counter_on * 1.0 / vd->locals.counter_total;
    if (eff_opening < VD_MIN_ON) {
        /* There is no spoon.  And no VICTOR. */
        vd->give_up = 1;
        return;
    }

    eff_angle = sens->current.direction /* FIXME: use sens->current.direction as soon as this is verified-working. */
        + vd->locals.weighted_sum / vd ->locals.counter_on;
    vd->victim_phi = determine_victim_phi(eff_angle - eff_opening / 2,
                                          eff_angle + eff_opening / 2,
                                          vd->locals.sensor_id);
    hal_send_victim_phi(vd->victim_phi);
    vd->victim_found = 1;
}

void vd_step(VDState* vd, Sensors* sens){
    switch(vd->locals.state) {
        case VD_off:
            entry_start(vd, sens);
            break;
        case VD_running:
            {
                const double angle = (hal_get_time() - vd->locals.time_begin)
                                     * SMC_ROT_PER_SEC / 1000.0;
                hal_debug_out(DEBUG_CAT_VD_HAVE_IR, sens->ir[vd->locals.sensor_id]);
                vd->locals.counter_total += 1;
                if (sens->ir[vd->locals.sensor_id]) {
                    vd->locals.counter_on += 1;
                    vd->locals.weighted_sum += angle;
                }
                if (angle >= 2 * M_PI) {
                    vd->locals.state = VD_done;
                    smc_halt();
                    compute_result(vd, sens);
                }
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
