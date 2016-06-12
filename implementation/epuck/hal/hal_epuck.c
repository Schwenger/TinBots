/*
 * E-Puck Hardware Abstraction Layer
 */

#include "hal.h"

#include "e_motors.h"

void set_speed_left(int speed) {
    e_set_speed_left(speed);
}

void set_speed_right(int speed) {
    e_set_speed_right(speed);
}
