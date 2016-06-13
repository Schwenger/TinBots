/*
 * Hardware Abstraction Layer
 */

#ifndef HAL_H
#define HAL_H

void set_speed_left(int speed);
void set_speed_right(int speed);

void set_led(unsigned int led, unsigned int value);

/* TODO: abstraction of Bluetooth communication */

void print(const char *message);

#endif
