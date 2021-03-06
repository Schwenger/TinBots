#ifndef MATLAB_H
#define MATLAB_H

#include "hal.h"
#include "map_heap.h"
#include "tinbot.h"

typedef struct MatlabBot MatlabBot;
typedef struct MatlabCom MatlabCom;

#define NUM_COMMUNICATION_BOTS 16
struct MatlabCom {
    unsigned int length;
    MatlabBot* bots[NUM_COMMUNICATION_BOTS];
};

struct MatlabBot {
    double motor_left;
    double motor_right;
    double raw_time;
    double debug_info[DEBUG_CAT_NUM];
    MapContainer map_container;
    unsigned int heartbeat_p;
    double heartbeat_time;

    MatlabCom* com;
    char* com_buf;
    unsigned int com_buf_used;

    TinBot* tinbot;
};

long matlab_create_bot(void);
void matlab_destroy_bot(long matlab_bot);
void matlab_select_bot(long matlab_bot, long matlab_com);

void matlab_loop(void);

double matlab_get_motor_left(void);
double matlab_get_motor_right(void);
double matlab_get_debug_info(int i);

void matlab_update_proximity(double* proximity);
void matlab_update_ir(double* ir);
void matlab_update_lps(double* lps);
void matlab_update_time(double time);
void matlab_update_attached(double attached);

long matlab_create_com(void);
void matlab_destroy_com(long matlab_com);

#endif
