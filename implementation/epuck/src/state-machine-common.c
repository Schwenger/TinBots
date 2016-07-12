#include "hal.h"
#include "pi.h"
#include "state-machine-common.h"

const double SMC_SENSE_TOL = 5;
const double SMC_SECS_PER_DEGREE = (M_PI / 180) / SMC_ROT_PER_SEC;

void smc_rot_left() {
    hal_set_speed(-SMC_MOTOR_ROT, SMC_MOTOR_ROT);
}

void smc_rot_right() {
    hal_set_speed(SMC_MOTOR_ROT, -SMC_MOTOR_ROT);
}

void smc_move() {
    hal_set_speed(SMC_MOTOR_MV, SMC_MOTOR_MV);
}

void smc_move_back() {
    hal_set_speed(-SMC_MOTOR_MV, -SMC_MOTOR_MV);
}

void smc_halt() {
    hal_set_speed(0, 0);
}

int smc_time_passed_p(const hal_time entered, const double wait_secs) {
    const hal_time now = hal_get_time();
    const hal_time wait_ticks = (hal_time)(wait_secs * 1000);
    const hal_time elapsed = now - entered;
    /* If this assert fails, you only need to fix this part.
     * Note that it won't fail for roughly 1193 hours (see e_time.h) */
    assert(now >= entered);
    return elapsed >= wait_ticks;
}
