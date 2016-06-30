void setup(TinBot* tinbot) {
    hal_print("Tin Bot Setup: VicDir");
    vd_reset(&tinbot->controller.vic_dir);
}

void loop(TinBot* tinbot) {
    vd_step(&tinbot->controller.vic_dir, &tinbot->sens);
    hal_debug_out(DEBUG_CAT_VD_STATE, tinbot->controller.vic_dir.state);
    hal_debug_out(DEBUG_CAT_VD_VICTIM_FOUND, tinbot->controller.vic_dir.victim_found);
    hal_debug_out(DEBUG_CAT_VD_VICTIM_PHI, tinbot->controller.vic_dir.victim_phi);
    hal_debug_out(DEBUG_CAT_VD_GIVE_UP, tinbot->controller.vic_dir.give_up);
}
