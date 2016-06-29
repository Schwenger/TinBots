#ifndef SRC_STATE_MACHINE_COMMON_H
#define SRC_STATE_MACHINE_COMMON_H

/* Some of these must be a macro, as xc16 is too dumb. */

extern const double SMC_SENSE_TOL;

#define SMC_MOTOR_MV (2)
#define SMC_MOTOR_ROT (1)

#define SMC_MV_PER_SEC SMC_MOTOR_MV
#define SMC_ROT_PER_SEC (SMC_MOTOR_ROT / 5.3)

extern const double SMC_SECS_PER_DEGREE;

void smc_rot_left(void);
void smc_rot_right(void);
void smc_move(void);
void smc_halt(void);

int smc_time_passed_p(const hal_time entered, const double wait_secs);

#endif
