#ifndef EPUCK_LEDS_H
#define EPUCK_LEDS_H

#include "p30F6014A.h"

#include "utils.h"

#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 3
#define LED4 4
#define LED5 5
#define LED6 6
#define LED7 7

#define LED0_STATUS _LATA6
#define LED1_STATUS _LATA7
#define LED2_STATUS _LATA9
#define LED3_STATUS _LATA12
#define LED4_STATUS _LATA10
#define LED5_STATUS _LATA13
#define LED6_STATUS _LATA14
#define LED7_STATUS _LATA15

#define LED0_DIR _TRISA6
#define LED1_DIR _TRISA7
#define LED2_DIR _TRISA9
#define LED3_DIR _TRISA12
#define LED4_DIR _TRISA10
#define LED5_DIR _TRISA13
#define LED6_DIR _TRISA14
#define LED7_DIR _TRISA15

#define FRONT_LED_STATUS _LATC1
#define FRONT_LED_DIR _TRISC1

#define BODY_LED_STATUS _LATC2
#define BODY_LED_DIR _TRISC2

void init_leds(void) {
    LED0_DIR = OUTPUT;
    LED1_DIR = OUTPUT;
    LED2_DIR = OUTPUT;
    LED3_DIR = OUTPUT;
    LED4_DIR = OUTPUT;
    LED5_DIR = OUTPUT;
    LED6_DIR = OUTPUT;
    LED7_DIR = OUTPUT;

    FRONT_LED_DIR = OUTPUT;

    BODY_LED_DIR = OUTPUT;
}

void set_led(unsigned int number, unsigned int value) {
    switch (number) {
        case 0:
            LED0_STATUS = value;
            break;
        case 1:
            LED1_STATUS = value;
            break;
        case 2:
            LED2_STATUS = value;
            break;
        case 3:
            LED3_STATUS = value;
            break;
        case 4:
            LED4_STATUS = value;
            break;
        case 5:
            LED5_STATUS = value;
            break;
        case 6:
            LED6_STATUS = value;
            break;
        case 7:
            LED7_STATUS = value;
            break;
    }
}

void set_front_led(unsigned int value) {
    FRONT_LED_STATUS = value;
}

void set_body_led(unsigned int value) {
    BODY_LED_STATUS = value;
}

#endif
