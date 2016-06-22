/*
 * E-Puck Hardware Abstraction Layer
 */

#include <string.h>

#include "hal.h"

/*
#include "e_uart_char.h"

#include <motor_led/advance_one_timer/e_motors.h>
#include <motor_led/advance_one_timer/e_led.h>
*/

void set_speed(double left, double right) {
    e_set_speed_left(left);
    e_set_speed_left(right);
}

void set_led(unsigned int led, unsigned int value) {
    e_set_led(led, value);
}

void print(const char* message) {
    /* FIXME
    e_send_uart1_char(message, strlen(message));
    while (e_uart1_sending());
    */
}
