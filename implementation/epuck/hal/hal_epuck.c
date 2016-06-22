/*
 * E-Puck Hardware Abstraction Layer
 */

#include <string.h> /* strlen */

#include "lib/motors.h"
#include "lib/leds.h"

#include "hal.h"

/*
#include "e_uart_char.h"
*/

void hal_set_speed(double left, double right) {
    //e_set_speed_left(left);
    //e_set_speed_left(right);
}

void hal_set_led(unsigned int led, unsigned int value) {
    e_set_led(led, value);
}

void hal_set_front_led(unsigned int value) {
    e_set_front_led(value);
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
