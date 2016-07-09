#include "t2t.h"
#include "t2t-parse.h"

/* ===== Sending ===== */

enum {
    /* Documentation at https://piratenpad.de/p/ZuCsUfLZOdj0ua */
    T2T_COMMAND_HEARTBEAT = 0x08,
    T2T_COMMAND_FOUND_PHI = 0x0A,
    T2T_COMMAND_FOUND_XY = 0x0B,
    T2T_COMMAND_UPDATE_MAP = 0x0C,
    T2T_COMMAND_DOCKED = 0x0D,
    T2T_COMMAND_COMPLETED = 0x0E,
    /* Use the reserved numbers so that clang complains in the event of a clash: */
    ZZZ_RESERVED_PRINT = 0x00, /* Also, SCAN */
    ZZZ_RESERVED_HELLO = 0x01,
    ZZZ_RESERVED_PHI_DEPRECATED = 0x02,
    ZZZ_RESERVED_LPS = 0x04,
    ZZZ_RESERVED_XY_DEPRECATED = 0x09,
    ZZZ_RESERVED_SET_LED = 0x80,
    ZZZ_RESERVED_SET_MOTOR = 0x81,
    ZZZ_RESERVED_TX_AMBIENT = 0x82,
    ZZZ_RESERVED_TX_REFLECTION = 0x83
};

void t2t_send_heartbeat(TinBot* bot) {
    (void)bot;
    hal_send_done(T2T_COMMAND_HEARTBEAT);
}

void t2t_send_found_phi(TinBot* bot) {
    /* FIXME */
}

void t2t_send_found_xy(TinBot* bot) {
    /* FIXME */
}

void t2t_send_update_map(TinBot* bot) {
    /* FIXME */
}

void t2t_send_docked(TinBot* bot) {
    (void)bot;
    hal_send_done(T2T_COMMAND_DOCKED);
}

void t2t_send_completed(TinBot* bot) {
    (void)bot;
    hal_send_done(T2T_COMMAND_COMPLETED);
}


/* ===== Receiving ===== */

void t2t_parse_heartbeat(TinBot* bot) {
    t2t_receive_heartbeat(bot);
}

void t2t_parse_found_phi(TinBot* bot, char* data, unsigned int length) {
    /* FIXME */
}

void t2t_parse_found_xy(TinBot* bot, int is_ours, char* data, unsigned int length) {
    /* FIXME */
}

void t2t_parse_update_map(TinBot* bot, int is_ours, char* data, unsigned int length) {
    /* FIXME */
}

void t2t_parse_docked(TinBot* bot) {
    t2t_receive_docked(bot);
}

void t2t_parse_completed(TinBot* bot) {
    t2t_receive_completed(bot);
}
