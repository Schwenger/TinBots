/**
 * E-Puck Communication Subsystem
 *
 */

#include "p30F6014A.h"

#include "utils.h"

static unsigned char address = 0;

void e_init_com(void) {
    // reset to known state
    U1MODE = 0;
    U1STA = 0;

    // direct interrupt on character reception
    U1STAbits.URXISEL = 0;

    // direct interrupt on character transmission
    U1STAbits.UTXISEL = 0;

    // set baudrate to 115200
    U1BRG = (unsigned int) ((FCY / ((float) 115200)) / 16.0 - 1);

    // clear TX interrupt flag
    IFS0bits.U1TXIF = 0;
    // enable TX interrupt
    //IEC0bits.U1TXIE = ON;

    // enable UART
    U1MODEbits.UARTEN = ON;

    // enable TX
    U1STAbits.UTXEN = ON;
}

/*
ISR(_U2RXInterrupt) {
    IFS1bits.U2RXIF = 0;

    static bool state = 0;
    state ^= 1;
    e_set_led(0, 1);
}

ISR(_U2TXInterrupt) {
    IFS1bits.U2TXIF = 0;
}
*/