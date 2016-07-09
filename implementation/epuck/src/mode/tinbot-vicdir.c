#include "../approximator.h"

void setup(TinBot* tinbot) {
    hal_print("Tin Bot Setup: VicDir");
    vd_reset(&tinbot->controller.vic_dir);
    approx_reset(&tinbot->controller.approx);
}

void loop(TinBot* tinbot) {
    approx_step(&tinbot->controller.approx, &tinbot->sens);

    if(tinbot->controller.approx.lps_initialized) {
        vd_step(&tinbot->controller.vic_dir, &tinbot->sens);
        hal_debug_out(DEBUG_CAT_VD_STATE, tinbot->controller.vic_dir.locals.state);
        hal_debug_out(DEBUG_CAT_VD_VICTIM_FOUND, tinbot->controller.vic_dir.victim_found);
        hal_debug_out(DEBUG_CAT_VD_VICTIM_PHI, tinbot->controller.vic_dir.victim_phi);
        hal_debug_out(DEBUG_CAT_VD_GIVE_UP, tinbot->controller.vic_dir.give_up);
        hal_debug_out(DEBUG_CAT_VD_ON_PERCENTAGE, tinbot->controller.vic_dir.locals.counter_on * 1.0 /
                                                  tinbot->controller.vic_dir.locals.counter_total);
        hal_debug_out(DEBUG_CAT_VD_AVG_ANGLE,
                      tinbot->controller.vic_dir.locals.weighted_sum / tinbot->controller.vic_dir.locals.counter_on);
    }
}
