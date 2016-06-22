#include "p30F6014A.h"

#include "utils.h"

#include "motors.h"
#include "scheduler.h"

#define MOTOR1_PHA_STATUS _LATD0
#define MOTOR1_PHB_STATUS _LATD1
#define MOTOR1_PHC_STATUS _LATD2
#define MOTOR1_PHD_STATUS _LATD3

#define MOTOR2_PHA_STATUS _LATD4
#define MOTOR2_PHB_STATUS _LATD5
#define MOTOR2_PHC_STATUS _LATD6
#define MOTOR2_PHD_STATUS _LATD7

#define MOTOR1_PHA_DIR _TRISD0
#define MOTOR1_PHB_DIR _TRISD1
#define MOTOR1_PHC_DIR _TRISD2
#define MOTOR1_PHD_DIR _TRISD3

#define MOTOR2_PHA_DIR _TRISD4
#define MOTOR2_PHB_DIR _TRISD5
#define MOTOR2_PHC_DIR _TRISD6
#define MOTOR2_PHD_DIR _TRISD7

static volatile int e_left_motor_phase = 0;
static volatile int e_right_motor_phase = 0;

static int e_left_speed = 0;
static int e_right_speed = 0;

static ETask* e_motor_left_task;
static ETask* e_motor_right_task;

void e_set_speed_left(int speed) {
    MOTOR1_PHA_STATUS = OFF;
    MOTOR1_PHB_STATUS = OFF;
    MOTOR1_PHC_STATUS = OFF;
    MOTOR1_PHD_STATUS = OFF;

    if (speed < -1000) {
        speed = -1000;
    } else if (speed > 1000) {
        speed = 1000;
    }

    e_left_speed = speed;

    if (speed < 0) {
        speed *= -1;
    }

    if (speed == 0) {
        e_task_deactivate(e_motor_left_task);
    } else {
        e_task_set_period(e_motor_left_task, (unsigned int) (10000 / speed));
        e_task_activate(e_motor_left_task);
    }
}

void e_set_speed_right(int speed) {
    MOTOR2_PHA_STATUS = OFF;
    MOTOR2_PHB_STATUS = OFF;
    MOTOR2_PHC_STATUS = OFF;
    MOTOR2_PHD_STATUS = OFF;

    if (speed < -1000) {
        speed = -1000;
    } else if (speed > 1000) {
        speed = 1000;
    }

    e_right_speed = speed;

    if (speed < 0) {
        speed *= -1;
    }

    if (speed == 0) {
        e_task_deactivate(e_motor_right_task);
    } else {
        e_task_set_period(e_motor_right_task, (unsigned int) (10000 / speed));
        e_task_activate(e_motor_right_task);
    }
}

void e_update_left_phase(void) {
    switch (e_left_motor_phase) {
        case 0:
            MOTOR1_PHA_STATUS = OFF;
            MOTOR1_PHB_STATUS = ON;
            MOTOR1_PHC_STATUS = OFF;
            MOTOR1_PHD_STATUS = ON;
            break;
        case 1:
            MOTOR1_PHA_STATUS = OFF;
            MOTOR1_PHB_STATUS = ON;
            MOTOR1_PHC_STATUS = ON;
            MOTOR1_PHD_STATUS = OFF;
            break;
        case 2:
            MOTOR1_PHA_STATUS = ON;
            MOTOR1_PHB_STATUS = OFF;
            MOTOR1_PHC_STATUS = ON;
            MOTOR1_PHD_STATUS = OFF;
            break;
        case 3:
            MOTOR1_PHA_STATUS = ON;
            MOTOR1_PHB_STATUS = OFF;
            MOTOR1_PHC_STATUS = OFF;
            MOTOR1_PHD_STATUS = ON;
            break;
    }
}

void e_update_right_phase(void) {
    switch (e_right_motor_phase) {
        case 0:
            MOTOR2_PHA_STATUS = OFF;
            MOTOR2_PHB_STATUS = ON;
            MOTOR2_PHC_STATUS = OFF;
            MOTOR2_PHD_STATUS = ON;
            break;
        case 1:
            MOTOR2_PHA_STATUS = OFF;
            MOTOR2_PHB_STATUS = ON;
            MOTOR2_PHC_STATUS = ON;
            MOTOR2_PHD_STATUS = OFF;
            break;
        case 2:
            MOTOR2_PHA_STATUS = ON;
            MOTOR2_PHB_STATUS = OFF;
            MOTOR2_PHC_STATUS = ON;
            MOTOR2_PHD_STATUS = OFF;
            break;
        case 3:
            MOTOR2_PHA_STATUS = ON;
            MOTOR2_PHB_STATUS = OFF;
            MOTOR2_PHC_STATUS = OFF;
            MOTOR2_PHD_STATUS = ON;
            break;
    }
}

void e_run_motor_left(void) {
    if (e_left_speed > 0) {
        e_left_motor_phase--;
        if (e_left_motor_phase < 0) e_left_motor_phase = 3;
    } else {
        e_left_motor_phase++;
        if (e_left_motor_phase > 3) e_left_motor_phase = 0;
    }
    e_update_left_phase();
}

void e_run_motor_right(void) {
    if (e_right_speed < 0) {
        e_right_motor_phase--;
        if (e_right_motor_phase < 0) e_right_motor_phase = 3;
    } else {
        e_right_motor_phase++;
        if (e_right_motor_phase > 3) e_right_motor_phase = 0;
    }
    e_update_right_phase();
}

void e_init_motors(void) {
    MOTOR1_PHA_DIR = OUTPUT;
    MOTOR1_PHB_DIR = OUTPUT;
    MOTOR1_PHC_DIR = OUTPUT;
    MOTOR1_PHD_DIR = OUTPUT;

    MOTOR2_PHA_DIR = OUTPUT;
    MOTOR2_PHB_DIR = OUTPUT;
    MOTOR2_PHC_DIR = OUTPUT;
    MOTOR2_PHD_DIR = OUTPUT;

    e_motor_left_task = e_task_create(e_run_motor_left, 0);
    e_motor_right_task = e_task_create(e_run_motor_right, 0);
}
