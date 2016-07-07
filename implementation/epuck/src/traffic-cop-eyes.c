#include <hal/hal.h>
#include <assert.h>
#include <pi.h>
#include "traffic-cop-eyes.h"

static double ray_dist(double x, double y, double phi);
static void enter_wait_detect(TCEState* tce, Sensors* sens);

enum TCE_STATES {
    TCE_noidea, /* no information about the victim's location */
    TCE_waitdetect, /* wait until next measurement takes place */
    TCE_waitoff, /* waited long enough, next measurement as early as possible, i.e. when next signal is detected */
    TCE_someidea, /* we have an idea about the victims angle, but not its position */
    TCE_done /* we know the victim's position */
};

static const int MIN_DIST = 12; /* arbitrary, TODO */

double ray_dist(double x, double y, double phi) {
    double ray_orth_x = -sin(phi);
    double ray_orth_y =  cos(phi);
    double dist = fabs(x * ray_orth_x + y * ray_orth_y); /* scalar product of [xy] and ray_orth */
    return dist;
}

void enter_wait_detect(TCEState* tce, Sensors* sens){
    tce->locals.state = TCE_waitdetect;
    tce->locals.last_x = sens->current.x;
    tce->locals.last_y = sens->current.y;
    tce->locals.last_phi = sens->current.direction;
    tce->need_angle = 1;
}

void tce_reset(TCEState* tce){
    tce->locals.state = TCE_noidea;
    tce->need_angle = 0;
}

void tce_step(TCEInputs* inputs, TCEState* tce, Sensors* sens){
    double dist; /* according to matlab this is supposed to be an output, but I do not know, why */

    if(inputs->found_victim_xy) {
        tce->locals.state = TCE_done;
        tce->need_angle = 0;
    }

    switch(tce->locals.state) {
        case TCE_noidea:
            if(inputs->ir_stable) {
                enter_wait_detect(tce, sens);
            }
            break;
        case TCE_waitdetect:
            if(inputs->found_victim_phi) {
                tce->locals.state = TCE_waitoff;
                tce->need_angle = 0;
            }
            break;
        case TCE_waitoff:
            if(inputs->found_victim_phi) {
                tce->locals.state = TCE_someidea;
            }
            break;
        case TCE_someidea:
            dist = ray_dist(sens->current.x - tce->locals.last_x, sens->current.y - tce->locals.last_y, tce->locals.last_phi);
            if(inputs->ir_stable && dist >= MIN_DIST) {
                enter_wait_detect(tce, sens);
            }
            break;
        case TCE_done:
            break;
        default:
            /* Uhh */
            hal_print("TrafficCop Eyes illegal state?!  Resetting ...");
            assert(0);
            tce_reset(tce);
            break;
    }
}
