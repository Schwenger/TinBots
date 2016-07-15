#include "controller.h"
#include "hal.h"

void controller_reset(Controller* c, Sensors* sens) {
    approx_reset(&c->approx, sens);
    blind_reset(&c->blind);
    mod_reset(&c->moderator);
    pa_reset(&c->pickup_artist);
    pe_reset(&c->path_exec);
    pf_reset(&c->path_finder);
    rhr_reset(&c->rhr);
    irs_reset(&c->ir_stab);
    tce_reset(&c->cop_eyes);
    vd_reset(&c->vic_dir);
    vf_reset(&c->vic_finder);
}

static void inquire_blind_decision(Controller* c, ControllerInput* in, Sensors* sens);
static void inquire_eyes_decision(Controller* c, Sensors* sens);
static void reset_appropriately(enum BlindRunChoice old_choice, Controller* c, Sensors* sens);
static void run_victim_finder(Controller* c, Sensors* sens);
static void run_path_finder_executer(Controller* c, Sensors* sens);

void controller_step(ControllerInput* in, Controller* c, Sensors* sens) {
    enum BlindRunChoice old_choice;

    /* FIXME: Run Moderator
    if (c->is_dead) {
        return;
    }
    */
    approx_step(&c->approx, sens);

    /* First, the eyes decide whether we need an interrupt. */
    inquire_eyes_decision(c, sens);

    /* Now the traffic cop decides "who is allowed to drive". */
    old_choice = c->blind.run_choice;
    inquire_blind_decision(c, in, sens);

    /* Do we need to reset any of the state machines? */
    if (old_choice != c->blind.run_choice) {
        reset_appropriately(old_choice, c, sens);
    }

    /* Now we know what to do. */
    switch (c->blind.run_choice) {
    case BLIND_RUN_CHOICE_none:
        /* Nothing to do here. */
        break;
    case BLIND_RUN_CHOICE_rhr:
        rhr_step(&c->rhr, sens);
        break;
    case BLIND_RUN_CHOICE_victim_finder:
        run_victim_finder(c, sens);
        break;
    case BLIND_RUN_CHOICE_pickup_artist:
        pa_step(&c->pickup_artist);
        break;
    case BLIND_RUN_CHOICE_path_finder:
        assert(!c->path_finder.no_path && !c->path_finder.path_completed);
        run_path_finder_executer(c, sens);
        break;
    default:
        /* Uhh */
        hal_print("Controller illegal state?!  Resetting ...");
        assert(0);
        controller_reset(c, sens);
        hal_set_speed(0, 0);
        break;
    }

    /* Update the internal map if necessary: */
    /* FIXME: pf_update_map(&c->path_finder, bluetooth_data) */

    /* Update the to-be-sent map if necessary: */
    /* FIXME: T2T_update_map(&c->path_finder, sens) */
}

static void inquire_blind_decision(Controller* c, ControllerInput* in, Sensors* sens) {
    BlindInputs inputs;
    inputs.found_victim_xy = c->vic_finder.found_victim_xy;
    inputs.need_angle = c->cop_eyes.need_angle;
    inputs.no_path = c->path_finder.no_path;
    inputs.path_completed = c->path_finder.path_completed;
    inputs.victim_attached = sens->victim_attached;
    inputs.origin_x = in->origin_x;
    inputs.origin_y = in->origin_y;
    inputs.victim_x = c->vic_finder.victim_x;
    inputs.victim_y = c->vic_finder.victim_y;
    blind_step(&inputs, &c->blind);
}

static void inquire_eyes_decision(Controller* c, Sensors* sens) {
    TCEInputs inputs;

    irs_step(&c->ir_stab, sens);

    inputs.found_victim_phi = c->vic_dir.victim_found;
    inputs.found_victim_xy = c->vic_finder.found_victim_xy;
    inputs.ray_phi = c->vic_dir.victim_phi;
    inputs.ir_stable = c->ir_stab.ir_stable;
    inputs.phi_give_up = c->vic_dir.give_up;
    tce_step(&inputs, &c->cop_eyes, sens);
}

static void reset_appropriately(enum BlindRunChoice old_choice, Controller* c, Sensors* sens) {
    VFInputs inputs;
    switch (old_choice) {
        case BLIND_RUN_CHOICE_none:
            /* Nothing to do here. */
            break;
        case BLIND_RUN_CHOICE_rhr:
            rhr_reset(&c->rhr);
            break;
        case BLIND_RUN_CHOICE_victim_finder:
            vd_reset(&c->vic_dir);
            /* Make sure that Victim Finder is *definitely* deactivated. */
            inputs.found_victim_phi = 0;
            vf_step(&inputs, &c->vic_finder, sens);
            break;
        case BLIND_RUN_CHOICE_pickup_artist:
            /* It will/should never run twice, but reset it anyways. */
            pa_reset(&c->pickup_artist);
            break;
        case BLIND_RUN_CHOICE_path_finder:
            pf_reset(&c->path_finder);
            pe_reset(&c->path_exec);
            break;
        default:
            /* Uhh, ignore that. */
            hal_print("Invalid previous state in Controller: Unknown blind cop choice.");
            break;
    }
}

static void run_path_finder_executer(Controller* c, Sensors* sens) {
    /* Q: Why don't we base the pf_input on pf's state?
     * A: Because that's unnecessary coupling.  Only read fields declared
     *    explicitly as output!  'state' is not an output. */
    PathFinderInputs pf_inputs;
    PathExecInputs pe_inputs;

    pf_inputs.compute = 1;
    pf_inputs.is_victim = c->blind.is_victim;
    pf_inputs.dest_x = c->blind.dst_x;
    pf_inputs.dest_y = c->blind.dst_y;
    pf_inputs.step_complete = c->path_exec.done;
    pf_inputs.step_see_obstacle = c->path_exec.see_obstacle;
    pf_inputs.map = map_get_accumulated();
    pf_step(&pf_inputs, &c->path_finder, sens);

    pe_inputs.drive = c->path_finder.drive;
    pe_inputs.backwards = c->path_finder.backwards;
    pe_inputs.next_x = c->path_finder.next.x;
    pe_inputs.next_y = c->path_finder.next.y;
    pe_step(&pe_inputs, &c->path_exec, sens);
}

static void run_victim_finder(Controller* c, Sensors* sens) {
    VFInputs inputs;
    vd_step(&c->vic_dir, sens);
    inputs.found_victim_phi = c->vic_dir.victim_found;
    inputs.victim_angle = c->vic_dir.victim_phi;
    vf_step(&inputs, &c->vic_finder, sens);
}
