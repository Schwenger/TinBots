#include "t2t.h"

/* Again, these are supposed to run within ISRs, so be quick about
 * copying these things into bot->* structures! */

void t2t_receive_heartbeat(TinBot* bot) {
    bot->sens.saw_heartbeat = 1;
}

void t2t_receive_found_phi(TinBot* bot, double x, double y, double phi) {
    /* FIXME */
    (void)bot;
    (void)x;
    (void)y;
}

void t2t_receive_found_xy(TinBot* bot, int is_ours, int x, int y, int iteration) {
    /* FIXME */
    (void)bot;
    (void)x;
    (void)y;
}

void t2t_receive_update_map(TinBot* bot, int x, int y, Map* map) {
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
