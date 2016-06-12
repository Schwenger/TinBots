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

#ifndef NEATAVR_PWM_HPP
#define NEATAVR_PWM_HPP

#include "core.hpp"

namespace NeatAVR {
    template<typename _Pin, typename _Channel, typename _Timer> class PWMMixin : public _Pin {
    public:
        class PWM {
        public:
            typedef _Channel Channel;
            typedef _Timer Timer;

            static inline void enable() ALWAYS_INLINE {
                _Channel::Mode::set(Channel::Mode::CLEAR);
            }

            static inline void set(uint8 value) ALWAYS_INLINE {
                _Channel::Compare::set(value);
            }
            static inline uint8 get() ALWAYS_INLINE {
                return _Channel::Compare::get_low();
            }
        };
    };
}

#endif
