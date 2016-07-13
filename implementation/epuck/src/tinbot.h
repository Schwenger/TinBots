#ifndef TINBOT_H
#define TINBOT_H

#include "controller.h"
#include "sensors.h"
#include "t2t-data.h"

typedef enum Mode {
    ALONE = 0,
    FULL = 1,
    RHR = 2,
    VICDIR = 3
} Mode;

typedef struct TinBot {
    Sensors sens;
    Controller controller;
    T2TData rx_buffer;
    Mode mode;
} TinBot;

typedef void (*Setup)(TinBot* tinbot);
typedef void (*Loop)(TinBot* tinbot);

typedef struct TinMode {
    Setup setup;
    Loop loop;
} TinMode;

void setup(TinBot* tinbot);
void loop(TinBot* tinbot);

void set_mode(TinBot* tinbot, Mode mode);

void update_proximity(TinBot* tinbot, double proximity[8]);
void update_ir(TinBot* tinbot, int ir[6]);
void update_lps(TinBot* tinbot, double x, double y, double phi);

void update_victim_pickup(TinBot* tinbot, unsigned int grabbed);
void update_victim_phi(TinBot* tinbot, double phi);

void set_victim_attached(TinBot* tinbot, unsigned int value);

#endif
