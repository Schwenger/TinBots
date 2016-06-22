#include "p30F6014A.h"

#include "utils.h"

#include "rs232.h"

void e_init_rs232(unsigned long baudrate) {
    // reset to known state
    U2MODE = 0;
    U2STA = 0;

    // direct interrupt on character reception
    U2STAbits.URXISEL = 0;

    // direct interrupt on character transmission
    U2STAbits.UTXISEL = 0;

    // set baudrate
    U2BRG = (unsigned int) ((FCY / ((float) baudrate)) / 16.0 - 1);

    // clear TX interrupt flag
    IFS1bits.U2TXIF = 0;
    // enable TX interrupt
    //IEC1bits.U2TXIE = ON;

    // enable UART
    U2MODEbits.UARTEN = ON;

    // enable TX
    U2STAbits.UTXEN = ON;
}

#include "leds.h"
/*
ISR(_U2TXInterrupt) {
    IFS1bits.U2TXIF = 0;
    static unsigned int state = 0;
    state ^= 1;
    e_set_led(5, state);
}*/