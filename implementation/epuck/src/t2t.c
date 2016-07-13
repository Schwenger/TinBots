#include <string.h>

#include "t2t.h"
#include "tinbot.h"

/* Again, these are supposed to run within ISRs, so be quick about
 * copying these things into bot->* structures! */

void t2t_receive_heartbeat(TinBot* bot) {
    bot->rx_buffer.newest_beat = hal_get_time();
}

void t2t_receive_found_phi(TinBot* bot, double x, double y, double phi) {
    /* FIXME */
    (void)bot;
    (void)x;
    (void)y;
}

void t2t_receive_phi_correction(struct TinBot* bot, double phi_correct, unsigned int acceptable) {
    /* FIXME */
    (void)bot;
    (void)phi_correct;
    (void)acceptable;
}

void t2t_receive_found_xy(TinBot* bot, int is_ours, int x, int y, int iteration) {
    /* FIXME */
    (void)bot;
    (void)x;
    (void)y;
}

void t2t_receive_update_map(TinBot* bot, int x, int y, Map* map) {
    /* The only one allowed to bypass the "t2t_pump" stuff. */
    (void)bot;
    map_merge(map_get_accumulated(), x, y, map);
}

void t2t_receive_docked(TinBot* bot) {
    /* Essentially switch off. */
    bot->controller.is_dead = 1;
}

void t2t_receive_completed(TinBot* bot) {
    /* Uhh, ignore that. */
    (void)bot;
}

void t2t_pump(TinBot* bot) {
    memcpy(&bot->sens.t2t, &bot->rx_buffer, sizeof(T2TData));
}
