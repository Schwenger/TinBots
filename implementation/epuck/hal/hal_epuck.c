/*
 * E-Puck Hardware Abstraction Layer
 */

#include <string.h>

#include "hal.h"

#include "e_uart_char.h"

#include <motor_led/advance_one_timer/e_motors.h>
#include <motor_led/advance_one_timer/e_led.h>

void set_speed_left(int speed) {
    e_set_speed_left(speed);
}

void set_speed_right(int speed) {
    e_set_speed_right(speed);
}

void set_led(unsigned int led, unsigned int value) {
    e_set_led(led, value);
}

void print(const char* message) {
    e_send_uart1_char(message, strlen(message));
    while (e_uart1_sending());
}
