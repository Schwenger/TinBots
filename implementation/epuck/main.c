/*
 * E-Puck Main Entry Point
 */

#include <string.h>
#include <stdio.h>

#include "tinpuck.h"

static volatile unsigned char bot_ir_sensors;

extern unsigned char com_buffer[64];

static TinPackage package;
extern TinPackage rx_package;


void debug_led_callback(TinPackage* package) {
    unsigned int number;
    tin_set_led(1, ON);
    for (number = 0; number < 8; number++) {
        tin_set_led(number, (((unsigned int) package->data[0]) >> number) & 1);
    }
}

void test_callback(TinPackage* package) {
    //tin_set_led(0, ON);
}

int main() {
    tin_init();

    tin_init_com();
    tin_init_rs232(9600UL);

    tin_wait(5);

    tin_calibrate_proximity();

    tin_com_register(0x10, debug_led_callback);

    char buffer[128];
    unsigned int index;
    unsigned int proximity[8];

    package.source = 0x42;
    package.target = 0x00;
    package.command = 0x60;
    package.length = 6;

    package.data = "Hello\n";
    package.callback = test_callback;

    while (1) {
        //asm volatile ("nop");
        // trigger ir sensor data refresh
        //I2CCONbits.SEN = 1;

        tin_wait(500);
        //tin_set_led(0, OFF);

        // WARNING: it is unsafe to send a package again before the corresponding callback has been called
        memset(buffer, 0, 128);
        sprintf(buffer, "data: %d %d %d %d\n", rx_package.source, rx_package.target, rx_package.command, rx_package.length);
        //tin_com_print(buffer);
        package.data = buffer;
        package.length = strlen(buffer);
        tin_com_send(&package);

        //memset(buffer, 0, 128);
        //sprintf(buffer, "%d\n", bot_ir_sensors);
        //tin_com_print(buffer);


        //idle_i2c();
        //I2CTRN= 0x42;
        //idle_i2c();
        //I2CCONbits.PEN=1;
        //idle_i2c();

        tin_get_proximity(proximity);
        //if(I2CSTATbits.P) {
        //    I2CCONbits.SEN = 1;
        //}

        //for (index = 0; index < 8; index++) {
        //    tin_set_led(index, proximity[index] > 15 ? ON : OFF);
        //}

        //memset(buffer, 0, 128);
        //sprintf(buffer, "%lu %u %u %u %u %u %u %u %u\n", tin_get_time(),
        //        proximity[0], proximity[1], proximity[2], proximity[3],
        //        proximity[4], proximity[5], proximity[6], proximity[7]);
        //tin_com_print(buffer);
        //memset(buffer, 0, 128);
        //sprintf(buffer, "%s\n", com_buffer);
        //tin_com_print(buffer);
    }
}

ISR(_MI2CInterrupt) {
    IFS0bits.MI2CIF=0;

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
            bot_ir_sensors = I2CRCV;
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
