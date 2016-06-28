/*
 * Hardware Abstraction Layer
 */

#ifndef HAL_H
#define HAL_H

#define BROADCAST_ADDRESS 0

/*
 * TODO: constants for LEDs and ON, OFF
 */

typedef unsigned long hal_time;

hal_time hal_get_time(void);

void hal_set_speed(double left, double right);

void hal_set_led(unsigned int led, unsigned int value);
void hal_set_front_led(unsigned int value);

void hal_send_msg(unsigned int address, char* message, unsigned int length);

void hal_print(const char *message);

typedef enum DebugCategory {
    DEBUG_CAT_RHR_STATE,
    DEBUG_CAT_OWN_TIME,
    DEBUG_CAT_NUM
} DebugCategory;

void hal_debug_out(DebugCategory key, double value);

#endif
