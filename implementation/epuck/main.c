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

#include "a_d/advance_ad_scan/e_ad_conv.h"
#include "a_d/advance_ad_scan/e_prox.h"

#include "I2C/e_I2C_protocol.h"

#include "utility/utility.h"

#include "src/tinbot.h"

#define EXT_I2C_ADDR 0x42
#define EXT_I2C_REG 0x00

char buffer[64];

int main() {
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
    int proximity[8];
    int ir[6];

    while (1) {
        for (number = 0; number < 8; number++) {
            proximity[number] = e_get_calibrated_prox(number);

        }
        memset(buffer, 0, 64);
        sprintf(buffer, "%i : %i\r\n", 0, proximity[0]);
        e_send_uart1_char(buffer, strlen(buffer));
        while (e_uart1_sending());
        e_send_uart2_char("Hallo", 5);
        while (e_uart2_sending());
        wait(800000);
        update_proximity(&tinbot, proximity);

        reg = e_i2cp_read(EXT_I2C_ADDR, EXT_I2C_REG);
        for (number = 0; number < 6; number++) {
            ir[number] = (reg >> number) & 1;
        }
        /*
        e_i2cp_write_string(EXT_I2C_ADDR, "Hallo", 0, 5);
         */
        e_i2c_start();
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
    }
}