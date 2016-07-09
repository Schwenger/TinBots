/*
 * E-Puck Hardware Abstraction Layer
 */

#include <string.h>
#include <stdio.h>
#include "sensors.h"
#include "tinpuck.h"

#include "hal.h"
#include "map.h"

typedef char check_map_proximity_size[(MAP_PROXIMITY_BUF_SIZE <= TIN_PACKAGE_MAX_LENGTH) ? 1 : -1];

static struct {
    double left, right;
} motor_wrapper;

const double ir_sensor_angle[6] = {
    7*M_PI/4,
    6*M_PI/4,
    5*M_PI/4,
    3*M_PI/4,
    2*M_PI/4,
    1*M_PI/4
};

void hal_set_speed(double left, double right) {
    tin_set_speed(left, right);
    motor_wrapper.left = left;
    motor_wrapper.right = right;
}

double hal_get_speed_left(void) {
    return motor_wrapper.left;
}

double hal_get_speed_right(void) {
    return motor_wrapper.right;
}

void hal_set_led(unsigned int led, unsigned int value) {
    tin_set_led(led, value);
}

void hal_set_front_led(unsigned int value) {
    tin_set_led(LED_FRONT, value);
}

hal_time hal_get_time() {
    return tin_get_time();
}

void hal_send_put(char* buf, unsigned int length) {
    /* FIXME */
}

void hal_send_done(char command) {
    /* FIXME */
}

void hal_print(const char* message) {
    static TinPackage package = {NULL, 0x00, 0x21, 0x00};
    package.data = (char*) message;
    package.length = strlen(message);
    tin_com_send(&package);
    while (!package.completed);
}

void hal_debug_out(DebugCategory key, double value) {
    /* Ignore, for now. */
    (void)key;
    (void)value;
}
