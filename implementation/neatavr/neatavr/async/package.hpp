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

#ifndef NEATAVR_PACKAGE_HPP
#define NEATAVR_PACKAGE_HPP

#ifndef PACKAGE_TIMEOUT
#define PACKAGE_TIMEOUT 10
#endif

#ifndef MAX_PACKAGE_LENGTH
#define MAX_PACKAGE_LENGTH 16
#endif

#ifndef MAX_PACKAGES
#define MAX_PACKAGES 5
#endif

#include <avr/interrupt.h>

#include "../core.hpp"
#include "../fifo.hpp"
#include "../serial.hpp"

#include "loop.hpp"
#include "task.hpp"

namespace NeatAVR {
    struct Package {
        uint8 command;
        uint8 length;
        char data[MAX_PACKAGE_LENGTH];
    };

    typedef FIFO<Package, MAX_PACKAGES> PackageFifo;

    PackageFifo fifo_rx;
    PackageFifo fifo_tx;

    Package current_rx;
    Package current_tx;

    volatile uint8 pointer_rx = 0;
    volatile uint8 pointer_tx = 0;

    enum EngineState {
        COMMAND,
        LENGTH,
        DATA,
        IDLE
    };

    volatile EngineState state_rx;
    volatile EngineState state_tx = IDLE;

    inline Package package_get() {
        return fifo_rx.get();
    }

    inline uint8 package_available() {
        return fifo_rx.available();
    }

    inline void send_package(Package package) ALWAYS_INLINE;
    inline void send_package(Package package) {
        fifo_tx.put(package);
        SYNCHRONIZED(SYNCHRONIZED_RESTORE) {
            if (state_tx == IDLE) {
                Serial::TX::Interrupt::enable();
                current_tx = fifo_tx.get();
                state_tx = (volatile EngineState) LENGTH;
                Serial::put(current_tx.command);
            }
        }
    }

    create_task(package_timeout, {
        sleep(PACKAGE_TIMEOUT);
        SYNCHRONIZED(SYNCHRONIZED_RESTORE) {
            state_rx = COMMAND;;
        }
    });

    ISR(SERIAL_RX_INTERRUPT) {
        switch (state_rx) {
            case COMMAND:
                current_rx.command = Serial::get();
                state_rx = LENGTH;
                reset_task(package_timeout);
                break;
            case LENGTH:
                current_rx.length = Serial::get();
                if (current_rx.length > MAX_PACKAGE_LENGTH) {
                    state_rx = COMMAND;
                    const Package response = {0x01, 0x00};
                    send_package(response);
                }
                if (current_rx.length > 0) {
                    state_rx = DATA;
                    pointer_rx = 0;
                } else {
                    fifo_rx.put(current_rx);
                    const Package response = {0x00, 0x00};
                    send_package(response);
                    state_rx = COMMAND;
                }
                break;
            case DATA:
                current_rx.data[pointer_rx] = (char) Serial::get();
                pointer_rx++;
                if (pointer_rx >= current_rx.length) {
                    fifo_rx.put(current_rx);
                    const Package response = {0x00, 0x00};
                    send_package(response);
                    state_rx = COMMAND;
                }
                break;
        }
    }

    ISR(SERIAL_TX_INTERRUPT) {
        switch (state_tx) {
            case COMMAND:
                Serial::put(current_tx.command);
                state_tx = LENGTH;
                break;
            case LENGTH:
                Serial::put(current_tx.length);
                if (current_tx.length > 0) {
                    state_tx = DATA;
                    pointer_tx = 0;
                } else {
                    if (fifo_tx.available()) {
                        current_tx = fifo_tx.get();
                        state_tx = COMMAND;
                    } else {
                        state_tx = IDLE;
                        Serial::TX::Interrupt::disable();
                    }
                }
                break;
            case DATA:
                Serial::put(current_tx.data[pointer_tx]);
                pointer_tx++;
                if (pointer_tx >= current_tx.length) {
                    if (fifo_tx.available()) {
                        current_tx = fifo_tx.get();
                        state_tx = COMMAND;
                    } else {
                        state_tx = IDLE;
                        Serial::TX::Interrupt::disable();
                    }
                }
                break;
            case IDLE:
                Serial::TX::Interrupt::disable();
                break;
        }
    }

    void package_init() {
        Serial::init(115200);
        Serial::RX::Interrupt::enable();
        Loop::attach(package_timeout);
        package_timeout->state = FINISHED;
    }
}

#endif
