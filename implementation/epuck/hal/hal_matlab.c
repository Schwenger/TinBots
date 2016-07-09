/*
 * Matlab Hardware Abstraction Layer
 *
 *
 * Communication Subsystem Example:
 *
 *  void hal_send_example(void) {
 *      unsigned int number;
 *      MatlabBot* bot = current;
 *      if (bot->com != NULL) {
 *          for (number = 0; number < bot->com->length; number++) {
 *              // select bot to send data to
 *              matlab_select_bot((long) bot->com->bots[number], (long) bot->com);
 *              // TODO: call appropriate receive function directly
 *          }
 *      }
 *      // select sending bot
 *      matlab_select_bot((long) bot, (long) bot->com);
 *  }
 */

#include <assert.h>
#include <math.h> /* Only for "time feedback" */
#include <stdlib.h>
#include <stdio.h>

#include "hal.h"

#include "matlab.h"
#include "tinbot.h"

/* Currently, "TinBot Software > Chart > Explore > debug_info > size" is set to 10 */
typedef char check_debug_array_length_against_matlab[(DEBUG_CAT_NUM <= 10) ? 1 : -1];

static MatlabBot* current;

const double ir_sensor_angle[6] = {
    0*M_PI/3,
    1*M_PI/3,
    2*M_PI/3,
    3*M_PI/3,
    4*M_PI/3,
    5*M_PI/3
};


/* Implementation of hal.h */

hal_time hal_get_time() {
    return (hal_time)(1000 * current->raw_time);
}

void hal_set_speed(double left, double right) {
    current->motor_left = left;
    current->motor_right = right;
}

double hal_get_speed_right(void) {
    return current->motor_right;
}

double hal_get_speed_left(void) {
    return current->motor_left;
}

void hal_set_led(unsigned int led, unsigned int value) {
    (void)led;
    (void)value;
    /* Ignore */
}
void hal_set_front_led(unsigned int value) {
    (void)value;
    /* Ignore */
}

void hal_send_put(char* to_add, unsigned int length) {
    /* FIXME */
}

void hal_print(const char* message) {
    static int have_fp = 0;
    static FILE* fp = 0;

    /* If it fails, prefer hanging over undefined behavior */
    while (!have_fp) {
        /* You will find this file in the same directory as "setup.m" */
        fp = fopen ("hal_print.txt","w");
        fputs("Open!\n", fp);
        have_fp = (fp != NULL);
    }
    fprintf(fp, "[%8.3f]: %s\n", current->raw_time, message);
    fflush(fp);
}

void hal_debug_out(DebugCategory key, double value) {
    assert(key < DEBUG_CAT_NUM);
    current->debug_info[key] = value;
    /* current->debug_info[DEBUG_CAT_OWN_TIME] = fmod(current->raw_time, 1); */
}


/* Implementation of matlab.h */

#define MAP_WIDTH 100
#define MAP_HEIGHT 100

long matlab_create_bot() {
    MatlabBot* matlab_bot = malloc(sizeof(MatlabBot));
    int i;
    matlab_bot->com = NULL;
    matlab_bot->tinbot = malloc(sizeof(TinBot));
    matlab_bot->map_container.accu = map_heap_alloc(MAP_WIDTH, MAP_HEIGHT);
    matlab_bot->map_container.prox = map_heap_alloc(MAP_PROXIMITY_SIZE, MAP_PROXIMITY_SIZE);
    /* setup() must be able to call hal_* functions: */
    matlab_select_bot((long)matlab_bot, 0);
    matlab_bot->raw_time = 0;
    setup(matlab_bot->tinbot);
    for (i = 0; i < DEBUG_CAT_NUM; ++i) {
        matlab_bot->debug_info[i] = 0.0 / 0.0;
    }
    return (long) matlab_bot;
}

void matlab_destroy_bot(long matlab_bot) {
    MatlabBot* bot = (MatlabBot*) matlab_bot;
    map_heap_free(bot->map_container.accu);
    map_heap_free(bot->map_container.prox);
    free(bot->tinbot);
    free(bot);
}

void matlab_select_bot(long matlab_bot, long matlab_com) {
    current = (MatlabBot*) matlab_bot;
    map_heap_container = &(current->map_container);
    if (!current->com && matlab_com != 0) {
        current->com = (MatlabCom*) matlab_com;
        assert(current->com->length < NUM_COMMUNICATION_BOTS);
        current->com->bots[current->com->length] = current;
        current->com->length++;
    }
}

void matlab_loop(void) {
    loop(current->tinbot);
}

double matlab_get_motor_left() {
    return current->motor_left;
}

double matlab_get_motor_right() {
    return current->motor_right;
}

double matlab_get_debug_info(int i) {
    if (i < 0 || i >= DEBUG_CAT_NUM) {
        return 0.0 / 0.0;
    }
    return current->debug_info[i];
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

void matlab_update_attached(double attached) {
    update_victim_pickup(current->tinbot, attached > 0);
}

void matlab_update_time(double time) {
    current->raw_time = time;
}


long matlab_create_com(void) {
    MatlabCom* matlab_com = malloc(sizeof(MatlabCom));
    matlab_com->length = 0;
    return (long) matlab_com;
}

void matlab_destroy_com(long matlab_com) {
    free((MatlabCom*) matlab_com);
}
