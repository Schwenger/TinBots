#include "moderator.h"
#include "state-machine-common.h"
#include "t2t.h"
#include "t2t-parse.h"

enum {
    MOD_STATE_SEARCHING,
    MOD_STATE_WAITING_BIDDING,
    MOD_STATE_WAITING_WATCHING,
    MOD_STATE_RESCUEING,
    MOD_STATE_DEAD
};

#define T2T_HEARTBEAT_TIMEOUT_SECS 9
typedef char check_heartbeat_timeout[(T2T_HEARTBEAT_TIMEOUT_SECS > 1 + T2T_HEARTBEAT_PERIOD_SECS) ? 1 : -1];

void mod_reset(ModState* mod) {
    mod->decision = MOD_DECISION_SEARCHING;
    mod->locals.time_entered = 0;
    mod->locals.state = MOD_STATE_SEARCHING;
    mod->locals.sent_iteration = -1;
}

void mod_step(ModInputs* inputs, ModState* mod) {
    int old_state;

    /* Make sure we never lag too far behind: */
    if (mod->locals.sent_iteration < inputs->t2t_data->newest_theirs - 1) {
        mod->locals.sent_iteration = inputs->t2t_data->newest_theirs - 1;
    }
    /* Obey Pickup Artist: */
    if (inputs->give_up || inputs->t2t_data->need_to_die) {
        mod->locals.state = MOD_STATE_DEAD;
    }

    old_state = mod->locals.state;
    switch (mod->locals.state) {
    case MOD_STATE_SEARCHING:
        if (inputs->t2t_data->newest_theirs >= 0) {
            /* Other Tin Bot was much faster. */
            mod->locals.sent_iteration = 0;
            mod->locals.state = MOD_STATE_WAITING_WATCHING;
            mod->decision = MOD_DECISION_WAITING;
        } else if (inputs->found_victim_xy) {
            t2t_send_found_xy((int)(inputs->own_victim_x + 0.5), (int)(inputs->own_victim_y + 0.5), 0);
            mod->locals.sent_iteration = 0;
            mod->locals.state = MOD_STATE_WAITING_BIDDING;
            mod->decision = MOD_DECISION_WAITING;
        }
        break;
    case MOD_STATE_WAITING_BIDDING:
        if (inputs->t2t_data->owning_xy_p) {
            /* "Whee!  My turn!" */
            mod->locals.state = MOD_STATE_RESCUEING;
            mod->decision = MOD_DECISION_RESCUEING;
        } else if (inputs->t2t_data->newest_theirs >= mod->locals.sent_iteration) {
            /* Other Tin Bot was faster. */
            mod->locals.sent_iteration = inputs->t2t_data->newest_theirs;
            mod->locals.state = MOD_STATE_WAITING_WATCHING;
            /* Should already be WAITING anyway. */
            mod->decision = MOD_DECISION_WAITING;
        }
        break;
    case MOD_STATE_WAITING_WATCHING:
        /* We don't have strong time guarantees on the main loop, so
         * *first* check whether there is a heartbeat "somewhere" in the
         * buffer, and only then check the time.
         * t2t.c guarantees us that no heartbeat is lost.  The exact time
         * of the beat is lost, but we know that it was "at some point
         * during the last main loop". */
        if (inputs->t2t_data->seen_beat) {
            /* Reset timer manually, as we can't change state. */
            mod->locals.time_entered = hal_get_time();
        } else if (smc_time_passed_p(mod->locals.time_entered, T2T_HEARTBEAT_TIMEOUT_SECS)) {
            /* They died.  Acknowledge the dead, and use the coordinates they
             * sent.  We do this because there's no guarantee that *we* ever
             * triangulated VICTOR successfully, and also would like to behave
             * "all in sync". */
            ++mod->locals.sent_iteration;
            t2t_send_found_xy(inputs->t2t_data->seen_x, inputs->t2t_data->seen_y, mod->locals.sent_iteration);
            mod->locals.state = MOD_STATE_WAITING_BIDDING;
            mod->decision = MOD_DECISION_WAITING;
        }
        break;
    case MOD_STATE_RESCUEING:
        if (!inputs->t2t_data->owning_xy_p) {
            /* Some other Tin Bot went wrong.  Stop. */
            smc_halt();
            mod->locals.state = MOD_STATE_DEAD;
        } else if (smc_time_passed_p(mod->locals.time_entered, T2T_HEARTBEAT_PERIOD_SECS)) {
            mod->locals.time_entered = hal_get_time();
            t2t_send_heartbeat();
        }
        break;
    case MOD_STATE_DEAD:
        /* Nothing to do here. */
        break;
    }

    if (mod->locals.state != old_state) {
        mod->locals.time_entered = hal_get_time();
    }
}
