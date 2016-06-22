#ifndef EPUCK_SCHEDULER_H
#define EPUCK_SCHEDULER_H

typedef struct ETask {
    unsigned int period;
    unsigned int counter;
    bool active;
    void (*callback)(void);
    struct ETask* next;
} ETask;

void e_init_scheduler(void);

void e_task_register(ETask* task, void (*callback)(void), unsigned int period);
ETask* e_task_create(void (*callback)(void), unsigned int period);
void e_task_activate(ETask* task);
void e_task_deactivate(ETask* task);
void e_task_set_period(ETask* task, unsigned int period);

#endif
