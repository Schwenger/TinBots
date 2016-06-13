/*
 * Tin Bot Controller Software
 */

#include "hal.h"

#include "tinbot.h"

void setup(TinBot* tinbot) {
    print("Tin Bot Setup");
    /*set_speed_left(500);
    set_speed_right(500);*/
}

void loop(TinBot* tinbot) {

}

void update_proximity(TinBot* tinbot, int proximity[8]) {
    unsigned int number;
    for (number = 0; number < 8; number++) {
        if (proximity[number] > 50) {
            set_led(number, 1);
        } else {
            set_led(number, 0);
        }

    }
}

void update_ir(TinBot* tinbot, int ir[6]) {

}

