#ifndef EPUCK_T2T_DATA_H
#define EPUCK_T2T_DATA_H

#include "hal.h"

typedef struct T2TData {
    /* Flags */
    hal_time newest_beat;
    unsigned char own_first_p;
    unsigned char someone_docked_p;
    unsigned char someone_completed_p;
    /* Counters */
    unsigned char newest_own;
    unsigned char newest_theirs;
} T2TData;

#endif
