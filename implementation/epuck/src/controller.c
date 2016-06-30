#include <assert.h>

#include "controller.h"
#include "hal.h"
#include "victim-finder.h"

void controller_reset(Controller* c) {
    blind_reset(&c->blind);
    pe_reset(&c->path_exec);
    rhr_reset(&c->rhr);
    tce_reset(&c->cop_eyes);
    vd_reset(&c->vic_dir);
    vf_reset(&c->vic_finder);

    /* In the first iteration, we look at the "previous" run choice of
     * the blind cop, so we simulate it. */
    c->blind.run_choice = BLIND_RUN_CHOICE_none;
}

static void inquire_blind_decision(Controller* c, ControllerInput* in);
static void inquire_eyes_decision(Controller* c, Sensors* sens);
static void reset_appropriately(enum BlindRunChoice old_choice, Controller* c, Sensors* sens);
static void run_victim_finder(Controller* c, Sensors* sens);
static void run_path_finder_executer(Controller* c, Sensors* sens);

void controller_step(ControllerInput* in, Controller* c, Sensors* sens) {

    enum BlindRunChoice old_choice;

    /* First, the eyes decide whether we need an interrupt. */
    inquire_eyes_decision(c, sens);

    /* Now the traffic cop decides "who is allowed to drive". */
    old_choice = c->blind.run_choice;
    inquire_blind_decision(c, in);

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
    case BLIND_RUN_CHOICE_path_finder:
        assert(!c->path_finder.no_path && !c->path_finder.path_completed);
        run_path_finder_executer(c, sens);
        break;
    default:
        /* Uhh */
        hal_print("Controller illegal state?!  Resetting ...");
        assert(0);
        controller_reset(c);
        hal_set_speed(0, 0);
        break;
    }

    /* Update the internal map if necessary: */
    /* FIXME: pf_update_map(&c->path_finder, sens) */
}

static void inquire_blind_decision(Controller* c, ControllerInput* in) {
    BlindInputs inputs;
    inputs.found_victim_phi = c->vic_dir.victim_found;
    inputs.found_victim_xy = c->vic_finder.found_victim_xy;
    inputs.need_angle = c->cop_eyes.need_angle;
    inputs.no_path = c->path_finder.no_path;
    inputs.path_completed = c->path_finder.path_completed;

    inputs.origin_x = in->origin_x;
    inputs.origin_y = in->origin_y;
    inputs.victim_x = c->vic_finder.victim_x;
    inputs.victim_y = c->vic_finder.victim_y;
    blind_step(&inputs, &c->blind);
}

static void inquire_eyes_decision(Controller* c, Sensors* sens) {
    TCEInputs inputs;
    inputs.found_victim_phi = c->vic_dir.victim_found;
    inputs.found_victim_xy = c->vic_finder.found_victim_xy;
    /* FIXME: Other cop-eyes inputs? */
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
        case BLIND_RUN_CHOICE_path_finder:
            pf_reset(&c->path_finder);
            break;
        default:
            /* Uhh, ignore that. */
            hal_print("Invalid state in Controller: Unknown blind cop choice.");
            break;
    }
}

static void run_path_finder_executer(Controller* c, Sensors* sens) {
    if(c->path_exec.done) {
        /* Alternatively, we could base the input on pf's state. */
        PathFinderInputs inputs;
        inputs.compute = 1;
        inputs.dest_x = c->blind.dst_x;
        inputs.dest_y = c->blind.dst_y;
        pf_step(&inputs, &c->path_finder, sens);
    } else {
        PathExecInputs inputs;
        inputs.drive_p = 1;
        inputs.next_x = c->path_finder.next.x;
        inputs.next_y = c->path_finder.next.y;
        pe_step(&inputs, &c->path_exec, sens);
    }
}

static void run_victim_finder(Controller* c, Sensors* sens) {
    VFInputs inputs;
    vd_step(&c->vic_dir, sens);
    inputs.found_victim_phi = c->vic_dir.victim_found;
    inputs.victim_angle = c->vic_dir.victim_phi;
    vf_step(&inputs, &c->vic_finder, sens);
}
