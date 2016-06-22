/*
 * E-Puck Main Entry Point
 */

#include <string.h>
#include <stdio.h>

#include "tinpuck.h"


static unsigned int send = 0;

void test(void) {
    static unsigned int state = 0;
    state ^= 1;
    tin_set_led(1, state);
    send = 1;
}



int main() {
    tin_init();

    tin_init_com();
    tin_init_rs232(9600UL);

    //e_init_time();






    //e_task_activate(e_task_create(test, 5000));

    //e_set_body_led(ON);
    //e_set_front_led(ON);

    //e_enable_interrupts();
    tin_set_speed(500, 500);
    //e_set_speed_left(500);
    //e_set_speed_right(500);

    unsigned int test = 0;
    while (1) {
        U2TXREG = 0x42;
        while (!U2STAbits.TRMT);
        test ^= 1;
        //e_set_led(6, test);
        U1TXREG = 0x42;
        while (!U1STAbits.TRMT);
        test ^= 1;
        //e_set_led(6, test);
    }
}

