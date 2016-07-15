#include <string.h>

#include "t2t.h"
#include "tinbot.h"

/* Again, these are supposed to run within ISRs, so be quick about
 * copying these things into bot->* structures! */

void t2t_receive_heartbeat(TinBot* bot) {
    bot->rx_buffer.moderate.seen_beat = 1;
}

void t2t_receive_found_phi(TinBot* bot, double x, double y, double phi) {
    bot->rx_buffer.vicdir.x = 1;
    bot->rx_buffer.vicdir.y = 1;
    bot->rx_buffer.vicdir.phi = 1;
    bot->rx_buffer.vicdir.new_p = 1;
}

void t2t_receive_phi_correction(struct TinBot* bot, double phi_correct, unsigned int acceptable) {
    bot->rx_buffer.fixdir.phi_correct = phi_correct;
    bot->rx_buffer.fixdir.acceptable = acceptable;
}

void t2t_receive_found_xy(TinBot* bot, int is_ours, int x, int y, int iteration) {
    struct T2TData_Moderate* buf = &bot->rx_buffer.moderate;

    if (buf->newest_own_INTERNAL == -1 && buf->newest_theirs == -1) {
        /* This will be executed exactly once with precisely the same
         * data on *every* Tin Bot.  (But not necessarily at the same
         * point in time.) */
        buf->seen_x = x;
        buf->seen_y = y;
    }

    if (is_ours) {
        if (iteration <= buf->newest_own_INTERNAL) {
            /* Whoops, outdated. */
            return;
        }
        buf->newest_own_INTERNAL = iteration;
    } else {
        if (iteration <= buf->newest_theirs) {
            /* Effectively outdated. */
            return;
        }
        buf->newest_theirs = iteration;
    }

    if (buf->newest_own_INTERNAL < buf->newest_theirs) {
        buf->owning_xy_p = 0;
    } else if (buf->newest_own_INTERNAL > buf->newest_theirs) {
        buf->owning_xy_p = 1;
    }
    /* Otherwise, don't update 'owning_xy_p' at all. */
}

void t2t_receive_update_map(TinBot* bot, int x, int y, Map* map) {
    /* The only one allowed to bypass the "t2t_pump" stuff. */
    (void)bot;
    map_merge(map_get_accumulated(), x, y, map);
}

void t2t_receive_docked(TinBot* bot) {
    /* Essentially switch off. */
    bot->rx_buffer.moderate.need_to_die = 1;
}

void t2t_receive_completed(TinBot* bot) {
    /* Uhh, ignore that. */
    (void)bot;
}

void t2t_data_pump(TinBot* bot) {
    memcpy(&bot->sens.t2t, &bot->rx_buffer, sizeof(T2TData));
    bot->rx_buffer.moderate.seen_beat = 0;
}

void t2t_data_init(T2TData* data) {
    memset(data, 0, sizeof(*data));
    data->moderate.newest_own_INTERNAL = -1;
    data->moderate.newest_theirs = -1;
}
