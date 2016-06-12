/*
 * Copyright (C) 2015, Maximilian Köhl <mail@koehlma.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NEATAVR_TASK_HPP
#define NEATAVR_TASK_HPP

#include <avr/pgmspace.h>

#define LABEL_START 65535
#define LABEL_END 65534

#define _pause(LABEL) { task->label = LABEL; task->state = PAUSING; return; case LABEL: ; }
#define pause() _pause(__COUNTER__)

#define _yield(LABEL) { task->label = LABEL; task->state = SLEEPING; task->data = 0; return; case LABEL: ; }
#define yield() _yield(__COUNTER__)

#define _sleep(MILLIS, LABEL) { task->label = LABEL; task->state = SLEEPING; task->data = MILLIS; return; case LABEL: ; }
#define sleep(MILLIS) _sleep(MILLIS, __COUNTER__);

#define _wait(CONDITION, LABEL)  { task->label = LABEL; task->state = WAITING; task->data = (uint16_t) (CONDITION); return; case LABEL: ; }
#define wait(CONDITION) _wait(CONDITION, __COUNTER__)

#define create_task(NAME, CODE)                                     \
    struct __Task_##NAME {                                          \
        static const Task task;                                     \
                                                                    \
        static void run() {                                         \
            task->state = RUNNING;                                  \
            switch (task->label) {                                  \
                case LABEL_START:                                   \
                    { CODE };                                       \
                case LABEL_END:                                     \
                    break;                                          \
            }                                                       \
            task->state = FINISHED;                                 \
        }                                                           \
    };                                                              \
                                                                    \
    _Task _##NAME = {CREATED, LABEL_START, 0, __Task_##NAME::run};  \
                                                                    \
    const Task PROGMEM NAME = &_##NAME;                             \
                                                                    \
    const Task PROGMEM __Task_##NAME::task = NAME;


#define create_coroutine(NAME, TASK, CODE)                      \
    struct __Coroutine_##NAME {                                 \
        static const Task task;                                 \
                                                                \
        static void run() {                                     \
            task->state = RUNNING;                              \
            switch (task->label) {                              \
                case LABEL_START:                               \
                    { CODE };                                   \
                case LABEL_END:                                 \
                    break;                                      \
            }                                                   \
            task->state = FINISHED;                             \
        }                                                       \
    };                                                          \
                                                                \
    const Task PROGMEM __Coroutine_##NAME::task = TASK;         \
    const Coroutine PROGMEM NAME = &__Coroutine_##NAME::run;

#define reset_task(TASK)            \
    {                               \
        TASK->state = CREATED;      \
        TASK->label = LABEL_START;  \
    }

namespace NeatAVR {
    enum State {
        CREATED,
        RUNNING,
        PAUSING,
        FINISHED,
        SLEEPING,
        WAITING
    };

    typedef void (*Coroutine)();
    typedef uint8_t (*Condition)();

    class _Task;
    typedef _Task *Task;

    class _Task {
    public:
        State state;

        uint16 label;
        uint16 data;

        Coroutine coroutine;

        void start() {
            this->label = 65535;
            this->coroutine();
        }

        void resume() {
            this->coroutine();
        }

        void set_coroutine(Coroutine coroutine) {
            this->state = CREATED;
            this->label = LABEL_START;
            this->coroutine = coroutine;
        }
    };
}

#endif
