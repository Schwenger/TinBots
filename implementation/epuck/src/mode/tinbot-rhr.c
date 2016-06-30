void setup(TinBot* tinbot) {
    hal_print("Tin Bot Setup: RHR");
    rhr_reset(&tinbot->rhr);
}

void loop(TinBot* tinbot) {
    rhr_step(&tinbot->rhr, &tinbot->sens);
}
