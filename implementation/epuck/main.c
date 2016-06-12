/*
 * E-Puck Main Entry Point
 */

#include "p30F6014A.h"

#include "e_init_port.h"
#include "e_motors.h"

#include "src/tinbot.h"

int main() {
    e_init_port();
    e_init_motors();

    setup();

    while (1) {
        // TODO: update proximity and ir data
        // update_ir();
        // update_proximity();
        loop();
    }
}