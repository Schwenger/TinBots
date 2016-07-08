/*
 * Tin Bot Controller Software
 */

#include <stdlib.h>
#include <time.h>

#include "hal.h"
#include "rhr.h"
#include "sensors.h"
#include "tinbot.h"

/* BEGIN CHOICE BLOCK
 * This is the actual implementation that will be executed.
 * Choose any of:
 * - mode/tinbot-rhr.c (works in matlab, works in reality)
 * - mode/tinbot-vicdir.c (not implemented)
 * - mode/tinbot-alone.c (not implemented)
 * - mode/tinbot-full.c (not implemented)
 */

#include "mode/tinbot-vicdir.c"

/* END CHOICE BLOCK */

void update_proximity(TinBot* tinbot, double proximity[8]) {
    unsigned int number;
    for (number = 0; number < 8; number++) {
        /* Q: Why doesn't do this any conversion?
         * A: Because the incoming values already are in cm. */
        tinbot->sens.proximity[number] = proximity[number];
    }
}

void update_ir(TinBot* tinbot, int ir[6]) {
    unsigned int i;
    for (i = 0; i < 6; ++i) {
        tinbot->sens.ir[i] = ir[i];
    }
    hal_set_led(1, (unsigned int) ir[0]);
    hal_set_led(2, (unsigned int) ir[1]);
    hal_set_led(3, (unsigned int) ir[2]);
    hal_set_led(5, (unsigned int) ir[3]);
    hal_set_led(6, (unsigned int) ir[4]);
    hal_set_led(7, (unsigned int) ir[5]);
}

void update_victim_pickup(TinBot* tinbot, unsigned int grabbed) {

}

void update_victim_phi(TinBot* tinbot, double phi) {

}

void update_lps(TinBot* tinbot, double x, double y, double direction) {
    tinbot->sens.lps.x = x;
    tinbot->sens.lps.y = y;
    tinbot->sens.lps.phi = direction;
}
