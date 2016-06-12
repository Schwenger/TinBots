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

#ifndef NEATAVR_LOOP_HPP
#define NEATAVR_LOOP_HPP

#include <avr/interrupt.h>

#include "../core.hpp"
#include "task.hpp"

#ifndef LOOP_TIMER
#define LOOP_TIMER Timer
#endif

#ifndef LOOP_PRESCALER
#define LOOP_PRESCALER LOOP_TIMER::Prescaler::DIV_64
#endif

#ifndef LOOP_INTERRUPT
#define LOOP_INTERRUPT TIMER_OVERFLOW_INTERRUPT
#endif

#ifndef LOOP_DIVIDER
#define LOOP_DIVIDER 64
#endif

#ifndef MAX_TASKS
#define MAX_TASKS 16
#endif

#define MILLIS_PER_OVERFLOW (255.0 / ((float) F_CPU / (float) LOOP_DIVIDER) * 1000)
#define MILLIS_PER_OVERFLOW_REMAINDER (((255.0 / ((float) F_CPU / (float) LOOP_DIVIDER) * 1000) - 1) * 1000)

namespace NeatAVR {
    Task tasks[MAX_TASKS];
    uint8 next = 0;

    volatile uint16 _millis = 0;
    volatile uint16 _remainder = 0;

    class Loop {
    public:
        static constexpr uint8 max_tasks = MAX_TASKS;

        static inline void init() ALWAYS_INLINE {
            LOOP_TIMER::Prescaler::set(LOOP_PRESCALER);
            LOOP_TIMER::Interrupt::enable();
        }

        static inline void attach(Task task) {
            tasks[next] = task;
            next++;
        }

        static inline void run() {
            System::enable_interrupts();
            Task task;

            while (1) {
                for (int number = 0; number < next; number++) {
                    task = tasks[number];
                    switch (task->state) {
                        case CREATED:
                            task->resume();
                            break;
                        case SLEEPING:
                            if (task->data == 0) {
                                task->resume();
                            }
                            break;
                        case WAITING:
                            if (((Condition) task->data)()) {
                                task->resume();
                            }
                            break;
                    }
                }
                uint16 millis;
                SYNCHRONIZED(SYNCHRONIZED_RESTORE) {
                    millis = _millis;
                }
                for (int number = 0; number < next; number++) {
                    task = tasks[number];
                    if (task->state == SLEEPING) {
                        if (task->data > millis) {
                            task->data -= millis;
                        } else {
                            task->data = 0;
                        }
                    }
                }
                SYNCHRONIZED(SYNCHRONIZED_RESTORE) {
                    _millis -= millis;
                }
            }
        }
    };

    ISR(LOOP_INTERRUPT) {
        _millis += MILLIS_PER_OVERFLOW;
        _remainder += MILLIS_PER_OVERFLOW_REMAINDER;
        while (_remainder >= 1000) {
            _millis++;
            _remainder -= 1000;
        }
    }
}

#endif
