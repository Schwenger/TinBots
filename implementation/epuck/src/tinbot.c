/*
 * Tin Bot Controller Software
 */

#include <stdlib.h>
#include <time.h>

#include "hal.h"
#include "rhr.h"

#include "tinbot.h"

void setup(TinBot* tinbot) {
    hal_print("Tin Bot Setup");
    rhr_reset(&tinbot->rhr);
    srand(time(NULL));
}

void loop(TinBot* tinbot) {
    rhr_step(&tinbot->rhr, &tinbot->sens);
}

static double prox_to_cm(double measured) {
    return measured; /* FIXME */
}

void update_proximity(TinBot* tinbot, double proximity[8]) {
    unsigned int number;
    for (number = 0; number < 8; number++) {
        /*if (proximity[number] > 50) {
            hal_set_led(number, 1);
        } else {
            hal_set_led(number, 0);
        }*/
        tinbot->sens.proximity[number] = prox_to_cm(proximity[number]);
    }
}

void update_ir(TinBot* tinbot, int ir[6]) {
    unsigned int i;
    for (i = 0; i < 6; ++i) {
        tinbot->sens.ir[i] = ir[i];
    }
}

void update_victim_pickup(TinBot* tinbot, unsigned int grabbed) {

}

void update_lps(TinBot* tinbot, double x, double y, double direction) {
    /* FIXME */
}
