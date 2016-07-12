#include "t2t.h"

/* Again, these are supposed to run within ISRs, so be quick about
 * copying these things into bot->* structures! */

void t2t_receive_heartbeat(TinBot* bot) {
    /* FIXME */
    (void)bot;
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

void t2t_receive_update_map(TinBot* bot, int is_ours, int x, int y, Map* map) {
    /* FIXME */
    (void)bot;
    (void)x;
    (void)y;
}

void t2t_receive_docked(TinBot* bot) {
    /* FIXME */
    (void)bot;
}

void t2t_receive_completed(TinBot* bot) {
    /* FIXME */
    (void)bot;
}
