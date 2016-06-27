/*
 * E-Puck Main Entry Point
 */

#include <string.h>
#include <stdio.h>

#include "tinpuck.h"

int main() {
    tin_init();

    tin_init_com();
    tin_init_rs232(9600UL);

    tin_wait(5);

    tin_calibrate_proximity();

    char buffer[128];
    unsigned int index;
    unsigned int proximity[8];

    while (1) {
        tin_wait(500);

        tin_get_proximity(proximity);

        for (index = 0; index < 8; index++) {
            tin_set_led(index, proximity[index] > 15 ? ON : OFF);
        }

        memset(buffer, 0, 128);
        sprintf(buffer, "%lu %u %u %u %u %u %u %u %u \n", tin_get_time(),
                proximity[0], proximity[1], proximity[2], proximity[3],
                proximity[4], proximity[5], proximity[6], proximity[7]);
        tin_com_print("====");
        tin_com_print(buffer);
    }
}

