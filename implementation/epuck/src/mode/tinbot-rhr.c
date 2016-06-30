void setup(TinBot* tinbot) {
    hal_print("Tin Bot Setup: RHR");
    rhr_reset(&tinbot->controller.rhr);
}

void loop(TinBot* tinbot) {
    rhr_step(&tinbot->controller.rhr, &tinbot->sens);
}
