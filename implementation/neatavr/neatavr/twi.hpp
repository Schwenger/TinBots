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

#ifndef NEATAVR_TWI_HPP
#define NEATAVR_TWI_HPP

#include <avr/interrupt.h>

#include "core.hpp"

#define TWI_INTERRUPT TWI_vect

namespace NeatAVR {
    class TWI {
    public:
        class Control: public Register8<_SFR_ADDR(TWCR)> {
        public:
            typedef Bit0 Interrupt;
            typedef Bit2 Enable;
            typedef Bit3 Collision;
            typedef Bit4 StopCondition;
            typedef Bit5 StartCondition;
            typedef Bit6 Acknowledge;
            typedef Bit7 Ready;
        };

        class Status {
        public:
            typedef Register8<_SFR_ADDR(TWSR)> _Status;

            static inline uint8 get() {
                return _Status::get() & 0xF8;
            }
        };

        typedef Register8<_SFR_ADDR(TWAR)> Address;
        typedef Register8<_SFR_ADDR(TWDR)> Data;

        typedef Control::Interrupt Interrupt;

        static inline void init(uint8 address) ALWAYS_INLINE {
            Address::set(address);
            enable();
        }

        static inline void enable() ALWAYS_INLINE {
            Control::Enable::set();
            Control::Acknowledge::set();
        }

        static inline void disable() ALWAYS_INLINE {
            Control::Enable::clear();
        }

        static inline uint8 status() ALWAYS_INLINE {
            return Status::get();
        }

        static inline void address(uint8 address) ALWAYS_INLINE {
            Address::set(address);
        }

        static inline uint8 address() ALWAYS_INLINE {
            return Address::get();
        }

        static inline uint8 collision() ALWAYS_INLINE {
            return Control::Collision::get();
        }

        static inline uint8 start_condition() ALWAYS_INLINE {
            return Control::StartCondition::get();
        }

        static inline uint8 stop_condition() ALWAYS_INLINE {
            return Control::StopCondition::get();
        }

        static inline uint8 ready() ALWAYS_INLINE {
            return Control::Ready::get();
        }

        static inline void clear() ALWAYS_INLINE {
            return Control::Ready::clear();
        }

        static inline void put(uint8 data) ALWAYS_INLINE {
            Data::set(data);
        }

        static inline uint8 get() ALWAYS_INLINE {
            return Data::get();
        }

        static inline void acknowledge() ALWAYS_INLINE {
            Control::Acknowledge::set();
        }
    };
}

#endif
