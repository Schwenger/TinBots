/*
 * E-Puck Main Entry Point
 */

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "tinpuck.h"

#include "tinbot.h"

#define STATE_STARTUP 0
#define STATE_RUNNING 1

static volatile int ir_data[8] = {0};
static volatile double lps_data[3] = {0};
static volatile unsigned lps_updated = 0;
static volatile unsigned int pickup_data;

static volatile unsigned int state = STATE_STARTUP;

static unsigned int proximity_raw[8] = {0};
static double proximity[8] = {0};

static TinBot bot;


static void com_on_hello(TinPackage* package) {
    static TinPackage response = {NULL, 0x00, 0x01, 0x00};
    tin_com_send(&response);
}

static void com_on_update_lps(TinPackage* package) {
    // static TinPackage response = {NULL, 0x00, 0x01, 0x00};
    // static char buffer[128];
    double x = (((unsigned int) package->data[0] << 8) | ((unsigned int) package->data[1] & 0xff)) / 100.0;
    double y = (((unsigned int) package->data[2] << 8) | ((unsigned int) package->data[3] & 0xff)) / 100.0;
    double phi = (((unsigned int) package->data[4] << 8) | ((unsigned int) package->data[5] & 0xff)) / 1000.0;
    // sprintf(buffer, "%f %f %f", x, y, phi);
    // response.data = buffer;
    // response.length = strlen(buffer);
    // tin_com_send(&response);
    lps_data[0] = x;
    lps_data[1] = y;
    lps_data[2] = phi;
    lps_updated = 1;
}

static void com_on_start(TinPackage* package) {
    lps_updated = 0;
    state = STATE_RUNNING;
}

static void debug_led_callback(TinPackage* package) {
    unsigned int number;
    tin_set_led(0, ON);
    for (number = 0; number < 8; number++) {
        tin_set_led(number, (((unsigned int) package->data[0]) >> number) & 1);
    }
}

static TinTask update_ext_data_task;

void update_ext_data(void) {
    I2CCONbits.SEN = 1;
}

static TinTask com_debug_task;

static void com_debug(void) {
    static TinPackage package = {NULL, 0x00, 0x11, 0x00};
    static char buffer[255];
    memset(buffer, 0, 255);
    sprintf(buffer, "%d %d %d %d %d %d %f %f %f %f %f %f %f %f %f %f %f %d",
            ir_data[0], ir_data[1], ir_data[2],
            ir_data[3], ir_data[4], ir_data[5],
            lps_data[0], lps_data[1], lps_data[2],
            proximity[0], proximity[1], proximity[2], proximity[3],
            proximity[4], proximity[5], proximity[6], proximity[7],
            pickup_data);
    package.data = buffer;
    package.length = strlen(buffer);
    tin_com_send(&package);
}

static void debug_set(TinPackage* package) {
    if (package->data[0]) {
        tin_task_activate(&com_debug_task);
    } else {
        tin_task_deactivate(&com_debug_task);
    }
}

static void debug_oneshot(TinPackage* package) {
    com_debug();
}

int main() {
    tin_init();

    tin_init_com();
    tin_init_rs232(9600UL);

    tin_wait(2000);

    tin_calibrate_proximity();

    tin_com_set_address(tin_get_selector());

    tin_task_register(&update_ext_data_task, update_ext_data, 500);
    tin_task_activate(&update_ext_data_task);

    tin_task_register(&com_debug_task, com_debug, 5000);

    tin_com_register(0x01, com_on_hello);
    tin_com_register(0x02, com_on_update_lps);
    tin_com_register(0x03, com_on_start);

    tin_com_register(0x10, debug_led_callback);
    tin_com_register(0x11, debug_set);
    tin_com_register(0x12, debug_oneshot);

    while (state == STATE_STARTUP);
    while (!lps_updated);

    setup(&bot);

    unsigned int index;
    double x, y, phi;

    while (1) {
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
            })
            update_lps(&bot, x, y, phi);
            lps_updated = 0;
        }
        update_ir(&bot, (int*) ir_data);
        update_victim_pickup(&bot, pickup_data);

        loop(&bot);
    }
}

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
            I2CCONbits.ACKDT=1;
            I2CCONbits.ACKEN=1;
            state = 3;
            break;
        case 3:
            I2CCONbits.PEN = 1;
            state = 4;
            break;
        case 4:
            //I2CCONbits.SEN = 1;
            state = 0;
            break;
    }
}
