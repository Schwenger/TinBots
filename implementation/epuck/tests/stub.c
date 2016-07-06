/*
 * STUB Hardware Abstraction Layer
 *
 * The name of this file appears quite often in the Makefile.
 */

#include <assert.h>

#include "hal.h"
#include "pi.h"

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


hal_time hal_get_time() {
    static hal_time last_time = 0;
    return ++last_time;
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

void hal_send_msg(unsigned int address, char* message, unsigned int length) {
    /* Nothing to do here. */
    (void)address;
    (void)message;
    (void)length;
}

void hal_print(const char* message) {
    /* Nothing to do here. */
    (void)message;
}

void hal_debug_out(DebugCategory key, double value) {
    /* Nothing to do here. */
    (void)key;
    (void)value;
}

void hal_send_victim_phi(double phi) {
    /* Nothing to do here. */
    (void)phi;
}
