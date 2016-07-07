#ifndef SRC_CONTROLLER_H
#define SRC_CONTROLLER_H

#include "blind-cop.h"
#include "ir-stabilizer.h"
#include "path-exec.h"
#include "path-finder.h"
#include "rhr.h"
#include "sensors.h"
#include "traffic-cop-eyes.h"
#include "victim-direction.h"
#include "victim-finder.h"
#include "approximator.h"

typedef struct Controller {
    BlindState blind;
    PathExecState path_exec;
    PathFinderState path_finder;
    RhrState rhr;
    IRSState ir_stab;
    TCEState cop_eyes;
    VDState vic_dir;
    VFState vic_finder;
    ApproxState approx;
} Controller;

typedef struct ControllerInput {
    double origin_x;
    double origin_y;
} ControllerInput;

void controller_reset(Controller* c);
void controller_step(ControllerInput* in, Controller* c, Sensors* sens);

#endif
