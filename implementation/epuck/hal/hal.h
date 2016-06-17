/*
 * Hardware Abstraction Layer
 */

#ifndef HAL_H
#define HAL_H

#define BROADCAST_ADDRESS 0

/*
 * TODO: constants for LEDs and ON, OFF
 */

void set_speed(double left, double right);

void set_led(unsigned int led, unsigned int value);
void set_front_led(unsigned int value);

void send_msg(unsigned int address, char* message, unsigned int length);

void print(const char *message);

#endif
