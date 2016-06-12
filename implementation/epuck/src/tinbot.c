/*
 * Tin Bot Controller Software
 */

#include "hal.h"

#include "tinbot.h"

void setup(TinBot* tinbot) {
    print("Tin Bot Setup");
}

void loop(TinBot* tinbot) {
    set_speed_left(2);
    set_speed_right(2);
}

void update_proximity(TinBot* tinbot, int proximity[8]) {

}

void update_ir(TinBot* tinbot, int ir[6]) {

}

