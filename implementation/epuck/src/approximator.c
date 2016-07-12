#include "approximator.h"
#include "hal.h"
#include "pi.h"

void approx_reset(ApproxState* approx, Sensors* sens) {
    approx->prev_motor_left = 0;
    approx->prev_motor_right = 0;
    approx->prev_t = hal_get_time();
    sens->current.x = 0;
    sens->current.y = 0;
    sens->current.phi = 0;
}

static const double tinbot_diameter = 5.3;

void approx_step(ApproxState* approx, Sensors* sens) {
    hal_time next_t;
    double delta_t, delta_phi, v, p;
    static unsigned int status = 0;
    next_t = hal_get_time();
    if(sens->lps.x != -1){
        status ^= 1;
        hal_set_front_led(status);
        sens->current.x = sens->lps.x;
        sens->current.y = sens->lps.y;
        sens->current.phi = sens->lps.phi;
        sens->lps.x = -1;
    } else {
        v = (approx->prev_motor_left + approx->prev_motor_right) / 2;
        p = (approx->prev_motor_right - approx->prev_motor_left) / tinbot_diameter;
        delta_t = (next_t - approx->prev_t) / 1000.0;
        delta_phi = p * delta_t;
        if(approx->prev_motor_right == approx->prev_motor_left){ /* i.e. p == 0 */
            sens->current.x += v * cos(sens->current.phi) * delta_t;
            sens->current.y += v * sin(sens->current.phi) * delta_t;
        } else {
            sens->current.x += v * (sin(delta_phi + sens->current.phi) - sin(sens->current.phi)) / p;
            sens->current.y += v * (cos(sens->current.phi) - cos(delta_phi + sens->current.phi)) / p;
        }
        sens->current.phi = fmod(sens->current.phi + p * delta_t, 2 * M_PI);
    }
    approx->prev_t = next_t;
    approx->prev_motor_left = hal_get_speed_left();
    approx->prev_motor_right = hal_get_speed_right();
}
