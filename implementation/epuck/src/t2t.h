#ifndef EPUCK_T2T_H
#define EPUCK_T2T_H

#include "map.h"
#include "tinbot.h"

/* All of the following function are intended to be run from withing an ISR.
 * Please make sure they are only called when the package is from the
 * right source! */

/* 'Our' packages: MAY (don't care)
 * 'Their' packages: MUST */
void t2t_receive_heartbeat(TinBot* bot);

/* 'Our' packages: MUST NOT
 * 'Their' packages: MUST */
void t2t_receive_found_phi(TinBot* bot, double x, double y, double phi);

/* 'Our' packages: MUST
 * 'Their' packages: MUST
 * Please set the 'is_ours' flag. */
void t2t_receive_found_xy(TinBot* bot, int is_ours, int x, int y, int iteration);

/* 'Our' packages: MUST
 * 'Their' packages: MUST
 * Please set the 'is_ours' flag. */
void t2t_receive_update_map(TinBot* bot, int is_ours, int x, int y, Map* map);

/* 'Our' packages: MUST NOT
 * 'Their' packages: MUST */
void t2t_receive_docked(TinBot* bot);

/* 'Our' packages: MAY (don't care)
 * 'Their' packages: MAY (don't care)
 * Actually, I don't see any reasonable way we could
 * ever *receive* that command. */
void t2t_receive_completed(TinBot* bot);

#endif
