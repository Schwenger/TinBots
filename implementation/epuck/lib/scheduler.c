#include <stdlib.h>

#include "p30F6014A.h"

#include "utils.h"

#include "scheduler.h"

static ETask* e_task_list_head = NULL;

void e_init_scheduler(void) {
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

void e_task_register(ETask* task, void (*callback)(void), unsigned int period) {
    task->active = 0;
    task->counter = 0;
    task->period = period;
    task->callback = callback;
    task->next = e_task_list_head;
    e_task_list_head = task;
}

ETask* e_task_create(void (*callback)(void), unsigned int period) {
    ETask* task = (ETask*)(malloc(sizeof(ETask)));
    task->active = 0;
    task->counter = 0;
    task->period = period;
    task->callback = callback;
    task->next = e_task_list_head;
    e_task_list_head = task;
    return task;
}



void e_task_activate(ETask* task) {
    task->counter = 0;
    task->active = 1;
}

void e_task_deactivate(ETask* task) {
    task->active = 0;
}

void e_task_set_period(ETask* task, unsigned int period) {
    task->counter = 0;
    task->period = period;
}

void __attribute__((interrupt, auto_psv)) _T2Interrupt(void) {
    IFS0bits.T2IF = 0;

    ETask* task = e_task_list_head;

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