#ifndef SRC_CONTROLLER_H
#define SRC_CONTROLLER_H

#include "blind-cop.h"
#include "path-exec.h"
#include "path-finder.h"
#include "rhr.h"
#include "sensors.h"
#include "traffic-cop-eyes.h"
#include "victim-direction.h"
/* FIXME: #include "victim-finder.h" */

typedef struct Controller {
    BlindState blind;
    PathExecState path_exec;
    /* FIXME: path-finder results? (path, no_path_p, path_completed_p) */
    RhrLocals rhr;
    TCEState cop_eyes;
    VDState vic_dir;
    /* FIXME: victim-finder state? */
} Controller;

typedef struct ControllerInput {
    double origin_x;
    double origin_y;
} ControllerInput;

void controller_reset(Controller* c);
void controller_step(ControllerInput* in, Controller* c, Sensors* sens);

#endif
