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
#include "lib/adc.h"

int main() {
    e_init_leds();
    e_init_selector();
    e_init_scheduler();
    e_init_adc();
    e_init_motors();

    e_set_body_led(ON);
    e_set_front_led(ON);

    ENABLE_INTERRUPTS();

    //set_speed_left(500);
    //set_speed_right(500);

    while (1) {
        unsigned int value = e_get_selector();
        //set_led(1, value & 1);
        //set_led(3, (value >> 1) & 1);
        //set_led(5, (value >> 2) & 1);
        //set_led(7, (value >> 3) & 1);
    }
}

