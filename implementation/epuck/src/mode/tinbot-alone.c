#include "controller.h"

void setup(TinBot* tinbot) {
    hal_print("Tin Bot Setup: Alone");
    controller_reset(&tinbot->controller, &tinbot->sens);
}

void loop(TinBot* tinbot) {
    ControllerInput input;
    /* FIXME: Use actual initial lps data */
    input.origin_x = 50;
    input.origin_x = 50;

    controller_step(&input, &tinbot->controller, &tinbot->sens);
}
