#ifndef TINBOT_H
#define TINBOT_H

#include "controller.h"
#include "sensors.h"

typedef struct TinBot {
    Sensors sens;
    Controller controller;
} TinBot;

void setup(TinBot* tinbot);
void loop(TinBot* tinbot);

void update_proximity(TinBot* tinbot, double proximity[8]);
void update_ir(TinBot* tinbot, int ir[6]);
void update_lps(TinBot* tinbot, double x, double y, double phi);

void update_victim_pickup(TinBot* tinbot, unsigned int grabbed);
void update_victim_phi(TinBot* tinbot, double phi);

void set_victim_attached(TinBot* tinbot, unsigned int value);

#endif
