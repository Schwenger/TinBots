/*
 * E-Puck Main Entry Point
 */

#include "p30F6014A.h"

#include "e_init_port.h"

#include "e_uart_char.h"
#include "e_motors.h"

#include "a_d/advance_ad_scan/e_ad_conv.h"
#include "a_d/advance_ad_scan/e_prox.h"

#include "I2C/e_I2C_protocol.h"

#include "src/tinbot.h"

#define EXT_I2C_ADDR 0x42
#define EXT_I2C_REG 0x00

int main() {
    e_init_port();
    e_init_motors();
    e_init_uart1();

    e_init_ad_scan(ALL_ADC);

    TinBot tinbot;

    setup(&tinbot);

    char reg;
    unsigned int number;
    int proximity[8];
    int ir[6];

    while (1) {
        for (number = 0; number < 8; number++) {
            proximity[number] = e_get_prox(number);
        }
        update_proximity(&tinbot, proximity);

        reg = e_i2cp_read(EXT_I2C_ADDR, EXT_I2C_REG);
        for (number = 0; number < 6; number++) {
            ir[number] = (reg >> number) & 1;
        }
        update_ir(&tinbot, ir);

        loop(&tinbot);
    }
}