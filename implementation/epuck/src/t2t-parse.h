#ifndef EPUCK_T2T_PARSE_H
#define EPUCK_T2T_PARSE_H

#include "map.h"
#include "tinbot.h"

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

/* ===== Sending ===== */

void t2t_send_heartbeat(void);

void t2t_send_found_phi(int x, int y, double victim_phi);

void t2t_send_found_xy(int x, int y, int iteration);

void t2t_send_update_map(int x, int y, Map* map);

void t2t_send_docked(void);

void t2t_send_completed(void);


/* ===== Receiving =====
 * Same calling convention as in t2t.h, as this calls those functions */

/* 'Our' packages: MAY (don't care)
 * 'Their' packages: MUST */
void t2t_parse_heartbeat(TinBot* bot);

/* 'Our' packages: MUST NOT
 * 'Their' packages: MUST */
void t2t_parse_found_phi(TinBot* bot, char* data, unsigned int length);

/* 'Our' packages: MUST
 * 'Their' packages: MUST
 * Please set the 'is_ours' flag. */
void t2t_parse_found_xy(TinBot* bot, int is_ours, char* data, unsigned int length);

/* 'Our' packages: MUST
 * 'Their' packages: MUST
 * Please set the 'is_ours' flag. */
void t2t_parse_update_map(TinBot* bot, int is_ours, char* data, unsigned int length);

/* 'Our' packages: MUST NOT
 * 'Their' packages: MUST */
void t2t_parse_docked(TinBot* bot);

/* 'Our' packages: MAY (don't care)
 * 'Their' packages: MAY (don't care)
 * Actually, I don't see any reasonable way we could
 * ever *receive* that command. */
void t2t_parse_completed(TinBot* bot);

#endif
