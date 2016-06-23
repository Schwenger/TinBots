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

    //tin_set_speed(500, 500);

    char buffer[128];
    unsigned int proximity[8];

    while (1) {
        //U2TXREG = 0x42;
        //while (!U2STAbits.TRMT);

        tin_wait(500);

        tin_get_proximity(proximity);
        memset(buffer, 0, 128);
        sprintf(buffer, "%lu %u\n", tin_get_time(), proximity[0]);
        tin_com_print(buffer);

        //while (tin_get_time() - start < 500);
    }
}

