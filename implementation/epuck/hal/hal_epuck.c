/*
 * E-Puck Hardware Abstraction Layer
 */

#include <string.h>
#include <stdio.h>
#include "sensors.h"
#include "tinpuck.h"

#include "hal.h"
#include "map.h"

#include "commands.h"

typedef char check_map_proximity_size[(MAP_PROXIMITY_BUF_SIZE < TIN_PACKAGE_MAX_LENGTH) ? 1 : -1];

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

void hal_set_powersafe(unsigned int enabled) {
    tin_set_motors_powersafe(enabled);
}

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

hal_time hal_get_time(void) {
    return tin_get_time();
}

static TinPackage send_buf = {0, 0, 0, 0, NULL, NULL, 0, NULL};
static int send_buf_sending = 0;

static void send_buf_wait(void) {
    if (send_buf_sending) {
        while (!send_buf.completed)
            /* Not good, so do "active-waiting".
             * - hal_print() is a bad idea, as it worsens the problem.
             * - hal_set_speed(0, 0) might interfere badly with some algorithms. */
            ;
        send_buf_sending = 0;
        send_buf.length = 0;
    }
}

void hal_send_put(char* buf, unsigned int length) {
    static char send_buf_buf[TIN_PACKAGE_MAX_LENGTH];

    send_buf_wait();
    send_buf.data = (char*)send_buf_buf;
    /* Two checks in case of overflow. */
    assert(length <= TIN_PACKAGE_MAX_LENGTH);
    assert(send_buf.length + length <= TIN_PACKAGE_MAX_LENGTH);
    memcpy(send_buf.data + send_buf.length, buf, length);
    send_buf.length += length;
}

void hal_send_done(char command, int is_broadcast) {
    send_buf_wait();
    send_buf.source = NULL; /* FIXME */
    /* All T2T packages are broadcast. */
    send_buf.target = is_broadcast ? 0xFF : 0x00;
    send_buf.command = command;
    send_buf.callback = NULL;
    send_buf.completed = 0;
    tin_com_send(&send_buf);
    /* Not touching 'hal_send_buf.next' */
    send_buf_sending = 1;
}

void hal_print(const char* message) {
    static TinPackage package = {0, 0, CMD_PRINT, 0, NULL, NULL, 0, NULL};
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

void __assert_hal(const char *msg, const char *file, int line) {
    char buffer[255];
    memset(buffer, 0, 255);
    sprintf(buffer, "Failure: %s (%s:%d)", msg, file, line);
    hal_print(buffer);
}
