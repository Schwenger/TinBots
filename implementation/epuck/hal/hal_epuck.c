/*
 * E-Puck Hardware Abstraction Layer
 */

#include <string.h>

#include "hal.h"

#include "e_motors.h"
#include "e_uart_char.h"

void set_speed_left(int speed) {
    e_set_speed_left(speed);
}

void set_speed_right(int speed) {
    e_set_speed_right(speed);
}

void print(const char* message) {
    e_send_uart1_char(message, strlen(message));
}
