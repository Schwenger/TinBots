#include <assert.h>
#include <hal/hal.h>
#include <pi.h>
#include "approximator.h"

void approx_reset(ApproxState* approx) {
    approx->locals.prev_motor_left = 0;
    approx->locals.prev_motor_right = 0;
    approx->locals.prev_t = hal_get_time();
    approx->locals.x = 0;
    approx->locals.y = 0;
    approx->locals.phi = 0;
    approx->lps_initialized = 0;
}

static const double magic_number_1 = 2.0; /* please rename */
static const double magic_number_2 = 5.3;

void approx_step(ApproxInputs* inputs, ApproxState* approx) {
    ApproxLocals* loc = &approx->locals;
    hal_time next_t;
    double delta_t, delta_phi, v, p;
    static unsigned int status = 0;
    next_t = hal_get_time();
    if(inputs->lps->x != -1){
        status ^= 1;
        hal_set_front_led(status);
        loc->x = inputs->lps->x;
        loc->y = inputs->lps->y;
        loc->phi = inputs->lps->phi;
        inputs->lps->x = -1;
        approx->lps_initialized = 1;
    } else {
        v = (loc->prev_motor_left + loc->prev_motor_right) / magic_number_1;
        p = (loc->prev_motor_right - loc->prev_motor_left) / magic_number_2;
        delta_t = (next_t - loc->prev_t) / 1000.0;
        delta_phi = p * delta_t;
        if(loc->prev_motor_right == loc->prev_motor_left){ /* i.e. p == 0 */
            loc->x += v * cos(loc->phi) * delta_t;
            loc->y += v * sin(loc->phi) * delta_t;
        } else {
            loc->x += v * (sin(delta_phi + loc->phi) - sin(loc->phi)) / p;
            loc->y += v * (cos(loc->phi) - cos(delta_phi + loc->phi)) / p;
        }
        loc->phi = fmod(loc->phi + p * delta_t, 2 * M_PI);
    }
    loc->prev_t = next_t;
    loc->prev_motor_left = inputs->motor_left;
    loc->prev_motor_right = inputs->motor_right;
    approx->current.x = loc->x;
    approx->current.y = loc->y;
    approx->current.phi = loc->phi;
}
