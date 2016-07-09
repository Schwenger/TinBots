#ifndef EPUCK_T2T_PARSE_H
#define EPUCK_T2T_PARSE_H

#include "tinbot.h"

/* ===== Sending ===== */

void t2t_send_heartbeat(TinBot* bot);

void t2t_send_found_phi(TinBot* bot);

void t2t_send_found_xy(TinBot* bot);

void t2t_send_update_map(TinBot* bot);

void t2t_send_docked(TinBot* bot);

void t2t_send_completed(TinBot* bot);


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
