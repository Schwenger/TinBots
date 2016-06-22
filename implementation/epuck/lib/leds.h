#ifndef EPUCK_LEDS_H
#define EPUCK_LEDS_H

#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 3
#define LED4 4
#define LED5 5
#define LED6 6
#define LED7 7

void e_init_leds(void);

void e_set_led(unsigned int number, unsigned int value);
void e_set_front_led(unsigned int value);
void e_set_body_led(unsigned int value);

#endif
