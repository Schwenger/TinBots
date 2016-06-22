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
#include "lib/com.h"
#include "lib/time.h"
#include "lib/rs232.h"

static unsigned int send = 0;

void test(void) {
    static unsigned int state = 0;
    state ^= 1;
    e_set_led(1, state);
    send = 1;
}



int main() {
    e_init_leds();
    e_init_selector();
    e_init_scheduler();
    //e_init_adc();
    e_init_motors();

    e_init_com();
    e_init_rs232(9600UL);

    //e_init_time();






    e_task_activate(e_task_create(test, 5000));

    //e_set_body_led(ON);
    //e_set_front_led(ON);

    //e_enable_interrupts();
    ENABLE_INTERRUPTS();

    //e_set_speed_left(500);
    //e_set_speed_right(500);

    unsigned int test = 0;
    while (1) {
        U2TXREG = 0x42;
        while (!U2STAbits.TRMT);
        test ^= 1;
        e_set_led(6, test);
        U1TXREG = 0x42;
        while (!U1STAbits.TRMT);
        test ^= 1;
        e_set_led(6, test);
    }
}

