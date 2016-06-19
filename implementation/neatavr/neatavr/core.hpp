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

#ifndef NEATAVR_CORE_HPP
#define NEATAVR_CORE_HPP

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/atomic.h>

#define ALWAYS_INLINE __attribute__((always_inline))

#define SYNCHRONIZED(TYPE) ATOMIC_BLOCK(TYPE)
#define SYNCHRONIZED_FORCEON ATOMIC_FORCEON
#define SYNCHRONIZED_RESTORE ATOMIC_RESTORESTATE

#define INTERRUPT_ROUTINE(VECTOR, ...) ISR(VECTOR, __VA_ARGS__)

namespace NeatAVR {
    typedef uint8_t uint8;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef uint64_t uint64;

    typedef int8_t int8;
    typedef int16_t int16;
    typedef int32_t int32;
    typedef int64_t int64;

    typedef uint8 byte;
}

#include "register.hpp"

#include "registers/tccr.hpp"
#include "registers/tcnt.hpp"
#include "registers/timsk.hpp"
#include "registers/ocr.hpp"

namespace NeatAVR {
    class StatusRegister : public Register8<_SFR_ADDR(SREG)> {
    public:
        typedef Bit0 Carry;
        typedef Bit1 Zero;
        typedef Bit2 Negative;
        typedef Bit3 Overflow;
        typedef Bit4 Signed;
        typedef Bit5 Adjust;
        typedef Bit6 Transfer;
        typedef Bit7 Interrupt;
    };

    class System {
    public:
        static inline void enable_interrupts() ALWAYS_INLINE {
            sei();
        }

        static inline void disable_interrupts() ALWAYS_INLINE {
            cli();
        }
    };
}

#endif
