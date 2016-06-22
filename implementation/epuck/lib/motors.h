#ifndef EPUCK_MOTORS_H
#define EPUCK_MOTORS_H

#include "utils.h"
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

static volatile int left_motor_phase = 0;
static volatile int right_motor_phase = 0;

static int left_speed = 0;
static int right_speed = 0;

static Task* motor_left_task;
static Task* motor_right_task;

void set_speed_left(int speed) {
    MOTOR1_PHA_STATUS = OFF;
    MOTOR1_PHB_STATUS = OFF;
    MOTOR1_PHC_STATUS = OFF;
    MOTOR1_PHD_STATUS = OFF;

    if (speed < -1000) {
        speed = -1000;
    } else if (speed > 1000) {
        speed = 1000;
    }

    left_speed = speed;

    if (speed < 0) {
        speed *= -1;
    }

    if (speed == 0) {
        task_deactivate(motor_left_task);
    } else {
        task_set_period(motor_left_task, (unsigned int) (10000 / speed));
        task_activate(motor_left_task);
    }
}

void set_speed_right(int speed) {
    MOTOR2_PHA_STATUS = OFF;
    MOTOR2_PHB_STATUS = OFF;
    MOTOR2_PHC_STATUS = OFF;
    MOTOR2_PHD_STATUS = OFF;

    if (speed < -1000) {
        speed = -1000;
    } else if (speed > 1000) {
        speed = 1000;
    }

    right_speed = speed;

    if (speed < 0) {
        speed *= -1;
    }

    if (speed == 0) {
        task_deactivate(motor_right_task);
    } else {
        task_set_period(motor_right_task, (unsigned int) (10000 / speed));
        task_activate(motor_right_task);
    }
}

void update_left_phase(void) {
    switch (left_motor_phase) {
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

void update_right_phase(void) {
    switch (right_motor_phase) {
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

static volatile unsigned int state__ = 0;

void run_motor_left(void) {
    state__ ^= 1;
    set_led(0, state__);
    if (left_speed > 0) {
        left_motor_phase--;
        if (left_motor_phase < 0) left_motor_phase = 3;
    } else {
        left_motor_phase++;
        if (left_motor_phase > 3) left_motor_phase = 0;
    }
    update_left_phase();
}

void run_motor_right(void) {
    if (right_speed < 0) {
        right_motor_phase--;
        if (right_motor_phase < 0) right_motor_phase = 3;
    } else {
        right_motor_phase++;
        if (right_motor_phase > 3) right_motor_phase = 0;
    }
    update_right_phase();
}

void init_motors(void) {
    MOTOR1_PHA_DIR = OUTPUT;
    MOTOR1_PHB_DIR = OUTPUT;
    MOTOR1_PHC_DIR = OUTPUT;
    MOTOR1_PHD_DIR = OUTPUT;

    MOTOR2_PHA_DIR = OUTPUT;
    MOTOR2_PHB_DIR = OUTPUT;
    MOTOR2_PHC_DIR = OUTPUT;
    MOTOR2_PHD_DIR = OUTPUT;

    motor_left_task = task_create(run_motor_left, 0);
    motor_right_task = task_create(run_motor_right, 0);
}



#endif
