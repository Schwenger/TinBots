#ifndef TIME_H
#define TIME_H

/* This only works for 1193-ish hours */
typedef unsigned long e_time_t;

e_time_t get_e_time(void);

extern const e_time_t E_TICKS_PER_SEC;

#endif
