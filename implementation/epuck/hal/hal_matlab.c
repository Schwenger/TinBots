/*
 * Matlab Hardware Abstraction Layer
 */

#include "hal.h"

typedef struct EPuck {
    int motor_left;
    int motor_right;
} EPuck;

static EPuck epuck;

void set_speed_left(int speed) {
    epuck.motor_left = speed;
}

void set_speed_right(int speed) {
    epuck.motor_right = speed;
}

void print(const char* message) {

}
