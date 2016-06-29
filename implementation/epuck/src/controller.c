#include <assert.h>

#include "controller.h"
#include "hal.h"

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

void controller_step(ControllerInput* in, Controller* c, Sensors* sens) {
    /* == Not yet incorporated ==
    PathExecState path_exec;
    FIXME: path-finder results?
    VDState vic_dir;
    FIXME: victim-finder state?
    */

    enum BlindRunChoice old_choice;

    /* First, the eyes decide whether we need an interrupt. */
    {
        TCEInputs inputs;
        inputs.found_victim_phi = c->vic_dir.victim_found;
        inputs.found_victim_xy = c->vic_finder.found_victim_xy;
        /* FIXME: Other cop-eyes inputs? */
        tce_step(&inputs, &c->cop_eyes, sens);
    }

    /* Now the traffic cop decides "who is allowed to drive". */
    old_choice = c->blind.run_choice;
    {
        BlindInputs inputs;
        inputs.found_victim_phi = c->vic_dir.victim_found;
        inputs.found_victim_xy = c->vic_finder.found_victim_xy;
        inputs.need_angle = c->cop_eyes.need_angle;
        /* FIXME: inputs.no_path = c->path_finder.no_path; */
        /* FIXME: inputs.path_completed = c->path_finder.path_completed; */
        inputs.origin_x = in->origin_x;
        inputs.origin_y = in->origin_y;
        inputs.victim_x = c->vic_finder.victim_x;
        inputs.victim_y = c->vic_finder.victim_y;
        blind_step(&inputs, &c->blind);
    }

    /* Do we need to reset any of the state machines? */
    if (old_choice != c->blind.run_choice) {
        switch (old_choice) {
        case BLIND_RUN_CHOICE_none:
            /* Nothing to do here. */
            break;
        case BLIND_RUN_CHOICE_rhr:
            rhr_reset(&c->rhr);
            break;
        case BLIND_RUN_CHOICE_victim_finder:
            vd_reset(&c->vic_dir);
            {
                /* Make sure that Victim Finder is *definitely* deactivated. */
                VFInputs inputs;
                inputs.found_victim_phi = 0;
                vf_step(&inputs, &c->vic_finder, sens);
            }
            break;
        case BLIND_RUN_CHOICE_path_finder:
            /* pf_reset(&c->path_finder); */
            /* FIXME */
            break;
        default:
            /* Uhh, ignore that. */
            break;
        }
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
        vd_step(&c->vic_dir, sens);
        {
            VFInputs inputs;
            inputs.found_victim_phi = c->vic_dir.victim_found;
            inputs.victim_angle = c->vic_dir.victim_phi;
            vf_step(&inputs, &c->vic_finder, sens);
        }
        break;
    case BLIND_RUN_CHOICE_path_finder:
        /* pf_step(&c->path_finder); */
        /* FIXME */
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
