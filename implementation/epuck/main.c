/*
 * E-Puck Main Entry Point
 */

#include <string.h>
#include <stdio.h>

#include "lib/config.h"
#include "lib/utils.h"
#include "lib/leds.h"
#include "lib/selector.h"
#include "lib/scheduler.h"
#include "lib/motors.h"

int main() {
    init_leds();
    init_selector();
    init_scheduler();
    init_motors();

    set_body_led(ON);
    set_front_led(ON);

    ENABLE_INTERRUPTS();

    set_speed_left(500);
    set_speed_right(500);

    while (1) {
        unsigned int value = get_selector();
        //set_led(1, value & 1);
        //set_led(3, (value >> 1) & 1);
        //set_led(5, (value >> 2) & 1);
        //set_led(7, (value >> 3) & 1);
    }
}

