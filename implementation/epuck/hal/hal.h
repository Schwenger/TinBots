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

enum {
    /* Documentation at https://piratenpad.de/p/ZuCsUfLZOdj0ua */
    T2T_COMMAND_HEARTBEAT = 0x08,
    T2T_COMMAND_FOUND_PHI = 0x0A,
    T2T_COMMAND_FOUND_XY = 0x0B,
    T2T_COMMAND_UPDATE_MAP = 0x0C,
    T2T_COMMAND_DOCKED = 0x0D,
    T2T_COMMAND_COMPLETED = 0x0E,
    /* Use the reserved numbers so that clang complains in the event of a clash: */
    ZZZ_RESERVED_PRINT = 0x00, /* Also, SCAN */
    ZZZ_RESERVED_HELLO = 0x01,
    ZZZ_RESERVED_PHI_DEPRECATED = 0x02,
    ZZZ_RESERVED_LPS = 0x04,
    ZZZ_RESERVED_XY_DEPRECATED = 0x09,
    ZZZ_RESERVED_SET_LED = 0x80,
    ZZZ_RESERVED_SET_MOTOR = 0x81,
    ZZZ_RESERVED_TX_AMBIENT = 0x82,
    ZZZ_RESERVED_TX_REFLECTION = 0x83
};

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
