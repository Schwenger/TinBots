/*
 * E-Puck Main Entry Point
 */


#include <string.h>
#include <stdio.h>

#include "p30F6014A.h"

#include "e_init_port.h"

#include "e_uart_char.h"


#include <motor_led/advance_one_timer/e_motors.h>
#include <motor_led/advance_one_timer/e_agenda.h>

#include <motor_led/advance_one_timer/e_led.h>

#include "a_d/advance_ad_scan/e_ad_conv.h"
#include "a_d/advance_ad_scan/e_prox.h"

#include "I2C/e_I2C_protocol.h"

#include "utility/utility.h"

#include "src/tinbot.h"

#define EXT_I2C_ADDR 0x42

void enable_timer() {
    // stop timer
    T3CONbits.TON = 0;
    // reset timer control register
    T3CON = 0;
    // prescaler divide by 1
    T3CONbits.TCKPS = 0;
    // clear timer value
    TMR3 = 0;
    // interrupt every 100us
    PR3 = (unsigned int)(0.1 * MILLISEC);
    // clear interrupt flag
    IFS0bits.T3IF = 0;
    // enable timer interrupt
    IEC0bits.T3IE = 1;
    // start timer
    T3CONbits.TON = 1;
}

volatile unsigned long time = 0;
volatile unsigned int counter = 0;

volatile unsigned int state;

void __attribute__((__interrupt__, auto_psv)) _T3Interrupt(void)  {
    // interrupt is executed every 100us
    //counter++;
    //if (counter > 10) {
    //    counter = 0;
        state ^= 1;
        e_set_led(1, state);
    //    time++;
    //}
}

void reset_time() {
    INTERRUPT_OFF();
    time = 0;
    counter = 0;
    INTERRUPT_ON();
}

long get_time() {
    INTERRUPT_OFF();
    long value = time;
    INTERRUPT_ON();
    return value;
}





volatile unsigned int state = 0;
unsigned int on = 0;


int main() {
    enable_timer();

    e_init_port();
    e_start_agendas_processing();
    e_init_motors();
    e_init_uart1();
    e_init_uart2(BAUD115200);
    e_i2c_init();
    e_i2c_enable();

    e_init_ad_scan(ALL_ADC);

    e_calibrate_ir();

    TinBot tinbot;

    setup(&tinbot);

    char reg;
    unsigned int number;
    double proximity[8];
    int ir[6];

    while (1) {
        /*e_i2c_start();
        //i2c_write(EXT_I2C_ADDR, "Hello World!");
        e_i2c_write(EXT_I2C_ADDR);
        e_i2c_write('H');
        e_i2c_write('e');
        e_i2c_write('l');
        e_i2c_write('l');
        e_i2c_write('o');
        e_i2c_stop();
        e_i2c_reset();
        wait(80000);
         */
        e_i2c_start();
        e_i2c_write(EXT_I2C_ADDR);
        e_i2c_restart();
        e_i2c_write(EXT_I2C_ADDR | 1);
        char data;
        e_i2c_read(&data);
        e_i2c_ack();
        e_i2c_read(&data);
        e_i2c_nack();
        e_i2c_stop();
        e_i2c_reset();
        wait(80000);
        e_set_led(1, on);
        //e_i2cp_read(EXT_I2C_REG, 0x16);

        on ^= 1;
        /*e_i2cp_write(EXT_I2C_ADDR | 1, 0x05, 0x50);
        wait(80000);*/
        /*for (number = 0; number < 8; number++) {
            proximity[number] = e_get_calibrated_prox(number);

        }
        memset(buffer, 0, 64);
        sprintf(buffer, "%i : %f\r\n", 0, proximity[0]);
        e_send_uart1_char(buffer, strlen(buffer));
        while (e_uart1_sending());
        e_send_uart2_char("Hallo", 5);
        while (e_uart2_sending());
        wait(800000);
        update_proximity(&tinbot, proximity);

        reg = e_i2cp_read(EXT_I2C_ADDR, EXT_I2C_REG);
        for (number = 0; number < 6; number++) {
            ir[number] = (reg >> number) & 1;
        }*/
        /*
        e_i2cp_write_string(EXT_I2C_ADDR, "Hallo", 0, 5);
         */
        /*e_i2c_start();
        e_i2c_write(EXT_I2C_ADDR);
        e_i2c_write('H');
        e_i2c_write('e');
        e_i2c_write('l');
        e_i2c_write('l');
        e_i2c_write('o');
        e_i2c_stop();
        e_i2c_reset();
        update_ir(&tinbot, ir);

        loop(&tinbot);
         */
    }
}

