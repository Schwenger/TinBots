/*
 * E-Puck Hardware Abstraction Layer
 */

#include <string.h> /* strlen */

#include "tinpuck.h"

#include "hal.h"

/*
#include "e_uart_char.h"
*/

void hal_set_speed(double left, double right) {
    //e_set_speed_left(left);
    //e_set_speed_left(right);
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

void hal_send_msg(unsigned int address, char* message, unsigned int length) {
    /* FIXME */
}

void hal_print(const char* message) {
    /* FIXME
    e_send_uart1_char(message, strlen(message));
    while (e_uart1_sending());
    */
}

void hal_debug_out(DebugCategory key, double value) {
    /* Ignore, for now. */
    (void)key;
    (void)value;
}
