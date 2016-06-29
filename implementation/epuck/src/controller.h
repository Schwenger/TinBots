#ifndef SRC_CONTROLLER_H
#define SRC_CONTROLLER_H

#include "blind-cop.h"
#include "path-exec.h"
#include "pathfinder.h"
#include "rhr.h"
#include "traffic-cop-eyes.h"
#include "victim-direction.h"
/* FIXME: #include "victim-finder.h" */

typedef struct Controller {
    BlindState blind;
    PathExecState path_exec;
    /* FIXME: path-finder results? */
    RhrLocals rhr;
    TCEState cop_eyes;
    VDState vic_dir;
    /* FIXME: victim-finder state? */
} Controller;

void controller_reset(Controller* c);
void controller_step(Controller* c);

#endif
