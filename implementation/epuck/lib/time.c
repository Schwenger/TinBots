#include "utils.h"

#include "scheduler.h"

#include "time.h"

static ETask e_time_task;

volatile long e_time = 0;

void e_update_time(void) {
    e_time++;
}

void e_init_time(void) {
    e_task_register(&e_time_task, e_update_time, 10);
}

unsigned long e_get_time() {
    SYNCHRONIZED({
        return (unsigned long) e_time;
    })
}
