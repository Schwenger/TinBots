#include <assert.h>
#include <hal/hal.h>
#include <pi.h>
#include "approximator.h"

void approx_reset(ApproxState* tce, Sensors* sens) {
    tce->locals.prev_motor_left = 0;
    tce->locals.prev_motor_right = 0;
    tce->locals.prev_t = hal_get_time();
    tce->locals.x = sens->current.x;
    tce->locals.y = sens->current.y;
    tce->locals.phi = sens->current.direction;
}

static const double magic_number_1 = 2.0; /* please rename */
static const double magic_number_2 = 5.3;

void approx_step(ApproxInputs* inputs, ApproxState* tce) {
    ApproxLocals* loc = &tce->locals;
    hal_time delta_t;
    double delta_phi, v, p;

    if(inputs->lps.x != -1){
        loc->x = inputs->lps.x;
        loc->y = inputs->lps.y;
        loc->phi = inputs->lps.phi;
    } else {
        v = (loc->prev_motor_left + loc->prev_motor_right) / magic_number_1;
        p = (loc->prev_motor_right - loc->prev_motor_left) / magic_number_2;
        delta_t = hal_get_time() - loc->prev_t;
        delta_phi = p * delta_t;
        if(loc->prev_motor_right == loc->prev_motor_left){ /* i.e. p == 0 */
            loc->x = v * cos(loc->phi) * delta_t + loc->x;
            loc->y = v * sin(loc->phi) * delta_t + loc->y;
        } else {
            loc->x += v * (sin(delta_phi + loc->phi) - sin(loc->phi)) / p;
            loc->y += v * (cos(loc->phi) - cos(delta_phi + loc->phi)) / p;
        }
        loc->phi = fmod(loc->phi + p * delta_t, 2 * M_PI);
    }
    loc->prev_t = hal_get_time();
    loc->prev_motor_left = inputs->motor_left;
    loc->prev_motor_right = inputs->motor_right;
    tce->current.x = loc->x;
    tce->current.y = loc->y;
    tce->current.phi = loc->phi;
}
