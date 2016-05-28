#include "tincontrol.h"

void Controller::update_sensors(double* proximity, double* ir) {
    this->motor_left = 1;
    this->motor_right = 1;
}

Controller* controller;

Controller* create_controller() {
    return new Controller;
}

void delete_controller(Controller* controller) {
    delete controller;
}

double get_motor_left(Controller* controller) {
    return controller->motor_left;
}

double get_motor_right(Controller* controller) {
    return controller->motor_right;
}

void update_sensors(Controller* controller, double* proximity, double* ir) { 
    controller->update_sensors(proximity, ir);
}
