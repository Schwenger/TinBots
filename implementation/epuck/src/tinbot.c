/*
 * Tin Bot Controller Software
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "hal.h"
#include "map.h"
#include "rhr.h"
#include "sensors.h"
#include "tinbot.h"

void update_proximity(TinBot* tinbot, double proximity[8]) {
    unsigned int number;
    for (number = 0; number < 8; number++) {
        /* Q: Why doesn't do this any conversion?
         * A: Because the incoming values already are in cm. */
        tinbot->sens.proximity[number] = proximity[number];
    }
}

void update_ir(TinBot* tinbot, int ir[6]) {
    unsigned int i;
    for (i = 0; i < 6; ++i) {
        tinbot->sens.ir[i] = ir[i];
    }
    hal_set_led(1, (unsigned int) ir[0]);
    hal_set_led(2, (unsigned int) ir[1]);
    hal_set_led(3, (unsigned int) ir[2]);
    hal_set_led(5, (unsigned int) ir[3]);
    hal_set_led(6, (unsigned int) ir[4]);
    hal_set_led(7, (unsigned int) ir[5]);
}

void update_victim_pickup(TinBot* tinbot, unsigned int grabbed) {
    tinbot->sens.victim_attached = !!grabbed;
}

void update_victim_phi(TinBot* tinbot, double phi) {

}

void update_lps(TinBot* tinbot, double x, double y, double phi) {
    tinbot->sens.lps.x = x;
    tinbot->sens.lps.y = y;
    tinbot->sens.lps.phi = phi;
}


/* Mode - Alone */
static void setup_alone(TinBot* tinbot) {
    hal_print("Tin Bot Setup: Alone");
    controller_reset(&tinbot->controller, &tinbot->sens);
}

static void loop_alone(TinBot* tinbot) {
    ControllerInput input;
    /* FIXME: Use actual initial lps data */
    input.origin_x = 50;
    input.origin_x = 50;

    controller_step(&input, &tinbot->controller, &tinbot->sens);
}


/* Mode - Full */
static void setup_full(TinBot* tinbot) {
    hal_print("Tin Bot Setup: Full");
    /* FIXME: NOT IMPLEMENTED */
}

static void loop_full(TinBot* tinbot) {
    /* FIXME: NOT IMPLEMENTED */
}


/* Mode - RHR */
static void setup_rhr(TinBot* tinbot) {
    hal_print("Tin Bot Setup: RHR");
    approx_reset(&tinbot->controller.approx, &tinbot->sens);
    rhr_reset(&tinbot->controller.rhr);
}

static void loop_rhr(TinBot* tinbot) {
    rhr_step(&tinbot->controller.rhr, &tinbot->sens);
}


/* Mode - VICDIR */
static void setup_vicdir(TinBot* tinbot) {
    hal_print("Tin Bot Setup: VicDir");
    vd_reset(&tinbot->controller.vic_dir);
    approx_reset(&tinbot->controller.approx, &tinbot->sens);
}

static void loop_vicdir(TinBot* tinbot) {
    approx_step(&tinbot->controller.approx, &tinbot->sens);

    vd_step(&tinbot->controller.vic_dir, &tinbot->sens);
    hal_debug_out(DEBUG_CAT_VD_STATE, tinbot->controller.vic_dir.locals.state);
    hal_debug_out(DEBUG_CAT_VD_VICTIM_FOUND, tinbot->controller.vic_dir.victim_found);
    hal_debug_out(DEBUG_CAT_VD_VICTIM_PHI, tinbot->controller.vic_dir.victim_phi);
    hal_debug_out(DEBUG_CAT_VD_GIVE_UP, tinbot->controller.vic_dir.give_up);
    hal_debug_out(DEBUG_CAT_VD_ON_PERCENTAGE, tinbot->controller.vic_dir.locals.counter_on * 1.0 /
                                              tinbot->controller.vic_dir.locals.counter_total);
    hal_debug_out(DEBUG_CAT_VD_AVG_ANGLE,
                  tinbot->controller.vic_dir.locals.weighted_sum / tinbot->controller.vic_dir.locals.counter_on);
}


/* mergeonly - Full */
static void setup_mergeonly(TinBot* tinbot) {
    hal_print("Tin Bot Setup: mergeonly");
}

static char mergeonly_printbuf[100];

static void loop_mergeonly(TinBot* tinbot) {
    static const long iterations = 10000;
    long i = 0;
    hal_time time = hal_get_time();
    do {
        map_merge(map_get_accumulated(), 4, 2, map_get_proximity());
    } while (++i < iterations);
    time = hal_get_time() - time;
    sprintf(mergeonly_printbuf, "merge_only: avg over %ld iter: %.3f us/iter",
        iterations,
        time * 1000.0 /* 1000 us/s */ / iterations);
    hal_print(mergeonly_printbuf);
}


static TinMode modes[] = {
        {setup_alone, loop_alone},
        {setup_full, loop_full},
        {setup_rhr, loop_rhr},
        {setup_mergeonly, loop_mergeonly},
        {setup_vicdir, loop_vicdir}
};

void setup(TinBot* tinbot) {
    tinbot->sens.victim_attached = 0;
    modes[tinbot->mode].setup(tinbot);
}

void loop(TinBot* tinbot) {
    modes[tinbot->mode].loop(tinbot);
}

void set_mode(TinBot* tinbot, Mode mode) {
    tinbot->mode = mode;
}
