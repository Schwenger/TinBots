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

#ifndef NEATAVR_PORT_HPP
#define NEATAVR_PORT_HPP

#include "core.hpp"
#include "register.hpp"

#define LOW 0
#define HIGH 1

#define INPUT 0
#define OUTPUT 1

#define _MAKE_PORT(LETTER) typedef Port<Register8<_SFR_ADDR(DDR##LETTER)>, Register8<_SFR_ADDR(PORT##LETTER)>, Register8<_SFR_ADDR(PIN##LETTER)>> Port##LETTER;

namespace NeatAVR {
    template<typename _Port, uint8 _number> class Pin {
    public:
        typedef _Port Port;

        static constexpr uint8 number = _number;

        static inline void output() ALWAYS_INLINE {
            Port::Direction::set_bit(number);
        }
        static inline void input() ALWAYS_INLINE {
            Port::Direction::clear_bit(number);
        }

        static inline uint8 direction() ALWAYS_INLINE {
            return Port::Direction::get_bit(number);
        }
        static inline void direction(uint8 direction) ALWAYS_INLINE {
            Port::Direction::bit(number, direction);
        }

        static inline void high() ALWAYS_INLINE {
            Port::Output::set_bit(number);
        }
        static inline void low() ALWAYS_INLINE {
            Port::Output::clear_bit(number);
        }
        static inline void toggle() ALWAYS_INLINE {
            Port::Output::toggle_bit(number);
        }

        static inline void on() ALWAYS_INLINE {
            high();
        }
        static inline void off() ALWAYS_INLINE {
            low();
        }

        static inline void pullup() ALWAYS_INLINE {
            high();
        }
        static inline void pulldown() ALWAYS_INLINE {
            low();
        }

        static inline uint8 read() ALWAYS_INLINE {
            return Port::Input::get_bit(number);
        }
        static inline void write(uint8 state) ALWAYS_INLINE {
            Port::Output::bit(number, state);
        }
    };

    template<typename _Direction, typename _Output, typename _Input> class Port {
    public:
        typedef _Direction Direction;
        typedef _Output Output;
        typedef _Input Input;

        typedef Pin<Port, 0> Pin0;
        typedef Pin<Port, 1> Pin1;
        typedef Pin<Port, 2> Pin2;
        typedef Pin<Port, 3> Pin3;
        typedef Pin<Port, 4> Pin4;
        typedef Pin<Port, 5> Pin5;
        typedef Pin<Port, 6> Pin6;
        typedef Pin<Port, 7> Pin7;
    };

#ifdef DDRA
    _MAKE_PORT(A);
#endif
#ifdef DDRB
    _MAKE_PORT(B);
#endif
#ifdef DDRC
    _MAKE_PORT(C);
#endif
#ifdef DDRD
    _MAKE_PORT(D);
#endif
#ifdef DDRE
    _MAKE_PORT(E);
#endif
#ifdef DDRF
    _MAKE_PORT(F);
#endif
#ifdef DDRG
    _MAKE_PORT(G);
#endif
#ifdef DDRH
    _MAKE_PORT(H);
#endif
#ifdef DDRI
    _MAKE_PORT(I);
#endif
#ifdef DDRJ
    _MAKE_PORT(J);
#endif
#ifdef DDRK
    _MAKE_PORT(K);
#endif
#ifdef DDRL
    _MAKE_PORT(L);
#endif
}

#endif
