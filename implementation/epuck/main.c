#include <string.h>
#include <math.h>

#include "tinpuck.h"

#include "tinbot.h"

#include "commands.h"

#define STATE_STARTUP 0
#define STATE_RUNNING 1

static volatile int ir_data[8] = {0};

static volatile double lps_data[3] = {0};
static volatile unsigned lps_updated = 0;

static volatile unsigned int pickup_data;

static volatile double victim_phi = -1;
static volatile unsigned int victim_phi_updated = 0;

static volatile unsigned int state = STATE_STARTUP;

static volatile unsigned int loop_counter = 0;
static volatile unsigned int loop_freq = 0;

static unsigned int proximity_raw[8] = {0};
static double proximity[8] = {0};

static volatile unsigned int do_reset = 0;
static volatile unsigned int do_calibration = 0;

static TinBot bot;

static char my_com_addr;


/* default mode executed on start command */
static Mode mode = ALONE;


/* periodically check for new data from extension board */
static TinTask update_ext_data_task;

void update_ext_data(void) {
    I2CCONbits.SEN = 1;
}


/* reset loop counter */
static TinTask reset_loop_counter_task;

void reset_loop_counter(void) {
    loop_freq = loop_counter;
    loop_counter = 0;
}


/* send heartbeat */
static TinTask send_heartbeat_task;

void send_heartbeat(void) {
    static TinPackage package = {NULL, (char) 0xFF, CMD_T2T_HEARTBEAT, 0, NULL, NULL};
    tin_com_send(&package);
}

void hal_set_heartbeat(unsigned int enabled) {
    if (enabled) {
        tin_task_activate(&send_heartbeat_task);
    } else {
        tin_task_deactivate(&send_heartbeat_task);
    }
}


/* control commands */
static void com_on_hello(TinPackage* package) {
    static TinPackage response = {NULL, NULL, CMD_HELLO, 0, NULL, NULL};
    static char data[4 + 2 + 2] __attribute__ ((aligned (4)));
    ((float*) data)[0] = 3.1415;
    ((int*) data)[2] = -42;
    ((unsigned int*) data)[3] = 4255;
    response.target = package->source;
    response.data = data;
    response.length = sizeof(data);
    tin_com_send(&response);
}

static void com_on_start(TinPackage* package) {
    lps_updated = 0;
    state = STATE_RUNNING;
}

static void com_on_reset(TinPackage* package) {
    do_reset = 1;
}

static void com_on_set_mode(TinPackage* package) {
    do_reset = 1;
    mode = (Mode) package->data[0];
}

static void com_on_update_lps(TinPackage* package) {
    lps_data[0] = ((float*) package->data)[0];
    lps_data[1] = ((float*) package->data)[1];
    lps_data[2] = ((float*) package->data)[2];
    lps_updated = 1;
}

static void com_on_proximity_calibrate(TinPackage* package) {
    do_calibration = 1;
}

/* debug commands */
static void debug_on_info(TinPackage* package) {
    static TinPackage response = {NULL, NULL, CMD_DEBUG_INFO, 0, NULL, NULL};
    static char data[4 * 11 + 2 + 6 + 1] __attribute__ ((aligned (4)));
    unsigned int number;
    for (number = 0; number < 8; number++) {
        ((float*) data)[number] = proximity[number];
    }
    for (number = 0; number < 3; number++) {
        ((float*) data)[number + 8] = lps_data[number];
    }
    ((unsigned int*) data)[2 * 11] = loop_freq;
    for (number = 0; number < 6; number++) {
        data[4 * 11 + 2 + number] = (char) ir_data[number];
    }
    data[4 * 11 + 2 + 6] = (char) pickup_data;
    response.target = package->source;
    response.data = data;
    response.length = sizeof(data);
    tin_com_send(&response);
}

static void debug_on_led(TinPackage *package) {
    unsigned int number;
    unsigned int mask = ((unsigned int*) package->data)[0];
    for (number = 0; number < 10; number++) {
        tin_set_led(number, ((mask >> number) & 1));
    }
}

static void debug_on_motors(TinPackage *package) {
    double speed_left = ((float*) package->data)[0];
    double speed_right = ((float*) package->data)[1];
    tin_set_speed(speed_left, speed_right);
}


/* Forwarding to T2T handlers */
static void com_on_t2t_heartbeat(TinPackage *package) {
    (void)package;
    t2t_parse_heartbeat(&bot);
}

static void com_on_t2t_victim_phi(TinPackage *package) {
    t2t_parse_found_phi(&bot, package->data, package->length);
}

static void com_on_t2t_victim_xy(TinPackage *package) {
    t2t_parse_found_xy(&bot, package->source == my_com_addr, package->data, package->length);
}

static void com_on_t2t_update_map(TinPackage *package) {
    t2t_parse_update_map(&bot, package->data, package->length);
}

static void com_on_t2t_docked(TinPackage *package) {
    (void)package;
    t2t_parse_docked(&bot);
}

static void com_on_t2t_completed(TinPackage *package) {
    (void)package;
    t2t_parse_completed(&bot);
}

static void com_register_t2t() {
    tin_com_register(CMD_T2T_HEARTBEAT, com_on_t2t_heartbeat);
    tin_com_register(CMD_T2T_VICTIM_PHI, com_on_t2t_victim_phi);
    tin_com_register(CMD_T2T_VICTIM_XY, com_on_t2t_victim_xy);
    tin_com_register(CMD_T2T_UPDATE_MAP, com_on_t2t_update_map);
    tin_com_register(CMD_T2T_DOCKED, com_on_t2t_docked);
    tin_com_register(CMD_T2T_COMPLETED, com_on_t2t_completed);
}


/* main loop */
int main() {
    tin_init();

    tin_init_com();
    tin_init_rs232(9600UL);

    my_com_addr = (char)tin_get_selector(); /* FIXME: Should be char */
    tin_com_set_address(my_com_addr);

    tin_task_register(&update_ext_data_task, update_ext_data, 500);
    tin_task_activate(&update_ext_data_task);

    tin_task_register(&reset_loop_counter_task, reset_loop_counter, 10000);
    tin_task_activate(&reset_loop_counter_task);

    tin_task_register(&send_heartbeat_task, send_heartbeat, 50000);

    tin_com_register(CMD_HELLO, com_on_hello);
    tin_com_register(CMD_START, com_on_start);
    tin_com_register(CMD_RESET, com_on_reset);

    tin_com_register(CMD_SET_MODE, com_on_set_mode);
    tin_com_register(CMD_UPDATE_LPS, com_on_update_lps);
    tin_com_register(CMD_PROXIMITY_CALIBRATE, com_on_proximity_calibrate);

    tin_com_register(CMD_DEBUG_INFO, debug_on_info);
    tin_com_register(CMD_DEBUG_LED, debug_on_led);
    tin_com_register(CMD_DEBUG_MOTORS, debug_on_motors);

    tin_wait(2000);

    tin_calibrate_proximity();

    do_reset = 1;

    unsigned int index;
    double x, y, phi;

    while (1) {
        if (do_reset) {
            tin_set_speed(0, 0);
            for (index = 0; index < 10; index++) {
                tin_set_led(index, OFF);
            }
            state = STATE_STARTUP;
            while (state == STATE_STARTUP);
            while (!lps_updated);
            set_mode(&bot, mode);
            setup(&bot);
            com_register_t2t();
            do_reset = 0;
        }
        if (do_calibration) {
            hal_print("Calibrating Proximity Sensors");
            tin_calibrate_proximity();
            do_calibration = 0;
        }

        tin_get_proximity(proximity_raw);
        for (index = 0; index < 8; index++) {
            // Empirically found function to convert proximity data into cm:
            // f(x) = 26.539 * x^(-0.4202)
            proximity[index] = 26.539 * pow(proximity_raw[index], -0.4202);
        }

        update_proximity(&bot, proximity);

        if (lps_updated) {
            SYNCHRONIZED({
                x= lps_data[0];
                y = lps_data[1];
                phi = lps_data[2];
                lps_updated = 0;
            })
            update_lps(&bot, x, y, phi);
        }
        if (victim_phi_updated) {
            SYNCHRONIZED({
                phi = victim_phi;
                victim_phi_updated = 0;
            })
            update_victim_phi(&bot, phi);
        }

        loop_counter++;

        loop(&bot);
    }
}


/* I2C receive interrupt */
ISR(_MI2CInterrupt) {
    IFS0bits.MI2CIF=0;

    unsigned int index, data;

    static unsigned int state = 0;

    switch (state) {
        case 0:
            I2CTRN= 0x42 | 1;
            state = 1;
            break;
        case 1:
            I2CCONbits.RCEN = 1;
            state = 2;
            break;
        case 2:
            data = I2CRCV;
            for (index = 0; index < 6; index++) {
                ir_data[index] = (data >> index) & 1;
            }
            pickup_data = (data >> 6) & 1;

            update_ir(&bot, (int*) ir_data);
            update_victim_pickup(&bot, pickup_data);

            I2CCONbits.ACKDT=1;
            I2CCONbits.ACKEN=1;
            state = 3;
            break;
        case 3:
            I2CCONbits.PEN = 1;
            state = 4;
            break;
        case 4:
            state = 0;
            break;
    }
}
