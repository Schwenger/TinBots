#ifndef SRC_CONTROLLER_H
#define SRC_CONTROLLER_H

#include "approximator.h"
#include "blind-cop.h"
#include "ir-stabilizer.h"
#include "moderator.h"
#include "path-exec.h"
#include "path-finder.h"
#include "pickup-artist.h"
#include "rhr.h"
#include "sensors.h"
#include "traffic-cop-eyes.h"
#include "victim-direction.h"
#include "victim-finder.h"

typedef struct Controller {
    ApproxState approx;
    BlindState blind;
    ModState moderator;
    PathExecState path_exec;
    PathFinderState path_finder;
    PickupState pickup_artist;
    RhrState rhr;
    IRSState ir_stab;
    TCEState cop_eyes;
    VDState vic_dir;
    VFState vic_finder;
} Controller;

typedef struct ControllerInput {
    double origin_x;
    double origin_y;
} ControllerInput;

void controller_reset(Controller* c, Sensors* sens);
void controller_step(ControllerInput* in, Controller* c, Sensors* sens);

#endif
