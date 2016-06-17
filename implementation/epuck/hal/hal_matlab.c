/*
 * Matlab Hardware Abstraction Layer
 */

#include <stdlib.h>

#include "hal.h"

#include "tinbot.h"
#include "matlab.h"

static MatlabBot* current;

void set_speed(double left, double right) {
    current->motor_left = left;
    current->motor_right = right;
}

void set_led(unsigned int led, unsigned int value) {
    
}

void print(const char* message) {

}


long matlab_create_bot() {
    MatlabBot* matlab_bot = malloc(sizeof(MatlabBot));
    matlab_bot->tinbot = malloc(sizeof(TinBot));
    setup(matlab_bot->tinbot);
    return (long) matlab_bot;
}

void matlab_destroy_bot(long matlab_bot) {
    free(((MatlabBot*) matlab_bot)->tinbot);
    free((MatlabBot*) matlab_bot);
}

void matlab_select_bot(long matlab_bot) {
    current = (MatlabBot*) matlab_bot;
}

void matlab_loop(long matlab_bot) {
    loop(current->tinbot);
}

double matlab_get_motor_left() {
    return current->motor_left;
}

double matlab_get_motor_right() {
    return current->motor_right;
}

void matlab_update_proximity(double* proximity) {
    update_proximity(current->tinbot, proximity);
}

void matlab_update_ir(double* ir_raw) {
    int number;
    int ir[6];
    for (number = 0; number < 6; number++) {
        ir[number] = (int) ir_raw[number];
    }
    update_ir(current->tinbot, ir);
}

void matlab_update_lps(double* lps) {
    update_lps(current->tinbot, lps[0], lps[1], lps[2]);
}
