#ifndef EPUCK_SCHEDULER_H
#define EPUCK_SCHEDULER_H

#include <stdlib.h>

#include "p30F6014A.h"

#include "utils.h"

typedef struct Task {
    unsigned int period;
    unsigned int counter;
    bool active;
    void (*callback)(void);
    struct Task* next;
} Task;

static Task* task_list_head = NULL;

void init_scheduler(void) {
    // reset timer control register
    T2CON = 0;
    // disable prescaler
    T2CONbits.TCKPS = 0;
    // clear timer value
    TMR2 = 0;
    // interrupt every 100us
    PR2 = (unsigned int)(100 * MICROSEC);
    // clear interrupt flag
    IFS0bits.T2IF = 0;
    // enable interrupt
    IEC0bits.T2IE = 1;
    // enable timer
    T2CONbits.TON = 1;
}

Task* task_create(void (*callback)(void), unsigned int period) {
    Task* task = (Task*)(malloc(sizeof(Task)));
    task->active = 0;
    task->counter = 0;
    task->period = period;
    task->callback = callback;
    task->next = task_list_head;
    task_list_head = task;
    return task;
}

void task_activate(Task* task) {
    task->counter = 0;
    task->active = 1;
}

void task_deactivate(Task* task) {
    task->active = 0;
}

void task_set_period(Task* task, unsigned int period) {
    task->counter = 0;
    task->period = period;
}

void __attribute__((interrupt, auto_psv)) _T2Interrupt(void) {
    IFS0bits.T2IF = 0;

    Task* task = task_list_head;

    while (task) {
        if (task->active) {
            task->counter++;
            if (task->counter >= task->period) {
                task->counter = 0;
                task->callback();
            }
        }
        task = task->next;
    }
}

#endif
