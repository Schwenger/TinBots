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

    while (1) {
        U2TXREG = 0x42;
        while (!U2STAbits.TRMT);
        U1TXREG = 0x42;
        while (!U1STAbits.TRMT);
    }
}

