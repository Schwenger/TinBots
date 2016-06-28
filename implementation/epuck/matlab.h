#ifndef MATLAB_H
#define MATLAB_H

#include "tinbot.h"

typedef struct MatlabBot {
    double motor_left;
    double motor_right;
    double raw_time;

    TinBot* tinbot;
} MatlabBot;

long matlab_create_bot(void);
void matlab_destroy_bot(long matlab_bot);
void matlab_select_bot(long matlab_bot);

void matlab_loop(void);

double matlab_get_motor_left(void);
double matlab_get_motor_right(void);

void matlab_update_proximity(double* proximity);
void matlab_update_ir(double* ir);
void matlab_update_lps(double* lps);
void matlab_update_time(double time);

#endif
