/*
 * STUB Hardware Abstraction Layer
 *
 * The name of this file appears quite often in the Makefile.
 */

#include <assert.h>
#include <stdio.h>

#include "hal.h"
#include "pi.h"
#include "sensors.h" /* Wanna define ir_sensors_angles */
#include "stub.h"

const double ir_sensor_angle[6] = {
    0*M_PI/3,
    1*M_PI/3,
    2*M_PI/3,
    3*M_PI/3,
    4*M_PI/3,
    5*M_PI/3
};

static double mot_l;
static double mot_r;

static hal_time global_clock = 0;

void tests_stub_tick(hal_time amount) {
    global_clock += amount;
}

hal_time hal_get_time() {
    return global_clock;
}

void hal_set_speed(double left, double right) {
    mot_l = left;
    mot_r = right;
}

double hal_get_speed_right(void) {
    return mot_r;
}

double hal_get_speed_left(void) {
    return mot_l;
}

void hal_set_led(unsigned int led, unsigned int value) {
    /* Nothing to do here. */
    (void)led;
    (void)value;
}
void hal_set_front_led(unsigned int value) {
    /* Nothing to do here. */
    (void)value;
}

void hal_send_put(char* buf, unsigned int length) {
    /* FIXME */
    assert(0);
    (void)buf;
    (void)length;
}

void hal_send_done(char command) {
    /* FIXME: properly stub out messages
     * - how do tests interact with this?
     * - tests can't switch modes (yet?)
     *   (should call t2t_receive_* or something)
     * - the call targets aren't implemented yet! */
    assert(0);
}

void hal_print(const char* message) {
    printf("%s\n", message);
}

static double debug_info[DEBUG_CAT_NUM] = {0};

void hal_debug_out(DebugCategory key, double value) {
    assert(key < DEBUG_CAT_NUM);
    debug_info[key] = value;
}

double tests_stub_get_debug(DebugCategory cat) {
    assert(cat < DEBUG_CAT_NUM);
    return debug_info[cat];
}
