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

double hal_get_speed_left(void);
double hal_get_speed_right(void);

void hal_set_led(unsigned int led, unsigned int value);
void hal_set_front_led(unsigned int value);

void hal_send_put(char* buf, unsigned int length);

void hal_send_done(char command);

void hal_print(const char *message);

typedef enum DebugCategory {
    DEBUG_CAT_VD_STATE,
    DEBUG_CAT_VD_VICTIM_FOUND,
    DEBUG_CAT_VD_VICTIM_PHI,
    DEBUG_CAT_VD_GIVE_UP,
    DEBUG_CAT_VD_ON_PERCENTAGE,
    DEBUG_CAT_VD_AVG_ANGLE,
    DEBUG_CAT_VD_HAVE_IR,
    DEBUG_CAT_VD_IR_ID,
    /* DEBUG_CAT_OWN_TIME, */
    DEBUG_CAT_NUM
} DebugCategory;

void hal_debug_out(DebugCategory key, double value);

#endif
