void setup(TinBot* tinbot) {
    hal_print("Tin Bot Setup: RHR");
    approx_reset(&tinbot->controller.approx, &tinbot->sens);
    rhr_reset(&tinbot->controller.rhr);
}

void loop(TinBot* tinbot) {
    rhr_step(&tinbot->controller.rhr, &tinbot->sens);
}
