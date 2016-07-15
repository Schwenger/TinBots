#ifndef EPUCK_T2T_DATA_H
#define EPUCK_T2T_DATA_H

#include "hal.h"

typedef struct T2TData_Moderate T2TData_Moderate;

typedef struct T2TData {
    struct T2TData_Moderate {
        /* Flags */
        unsigned int need_to_die;
        unsigned int seen_beat;
        unsigned int owning_xy_p;
        unsigned int someone_docked_p;
        unsigned int someone_completed_p;
        /* Data */
        int seen_x;
        int seen_y;
        /* newest_* must both be signed types */
        int newest_own_INTERNAL; /* Move to own struct? */
        int newest_theirs;
    } moderate;
    struct {
        /* If 'new' data gets lost, that's okay, one of the two
         * senders got the other's info. */
        double x;
        double y;
        double phi;
        unsigned int new_p;
    } vicdir;
    struct {
        double phi_correct;
        unsigned int acceptable;
    } fixdir;
} T2TData;

void t2t_data_init(T2TData* data);

#endif
