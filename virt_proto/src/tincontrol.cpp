#include "tincontrol.h"

void Controller::update_sensors(double* proximity, double* ir) {
    this->motor_left = 1;
    this->motor_right = 1;
}

void update_sensors(Controller* controller, double* proximity, double* ir) { 
    controller->update_sensors(proximity, ir);
}
