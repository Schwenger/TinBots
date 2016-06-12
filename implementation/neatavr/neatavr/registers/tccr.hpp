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

#ifndef NEATAVR_TCCR_HPP
#define NEATAVR_TCCR_HPP

#include <avr/io.h>

#include "../register.hpp"

#define _MAKE_TCCR_ABC(NUMBER)                              \
    class Tccr##NUMBER {                                    \
    public:                                                 \
        typedef Register8<_SFR_ADDR(TCCR##NUMBER##A)> A;    \
        typedef Register8<_SFR_ADDR(TCCR##NUMBER##B)> B;    \
        typedef Register8<_SFR_ADDR(TCCR##NUMBER##C)> C;    \
    }

#define _MAKE_TCCR_AB(NUMBER)                               \
    class Tccr##NUMBER {                                    \
    public:                                                 \
        typedef Register8<_SFR_ADDR(TCCR##NUMBER##A)> A;    \
        typedef Register8<_SFR_ADDR(TCCR##NUMBER##B)> B;    \
    }

#define _MAKE_TCCR(NUMBER) typedef Register8<_SFR_ADDR(TCCR##NUMBER)> Tccr##NUMBER;

namespace NeatAVR {
    namespace Registers {

#if defined(TCCR0A) && defined(TCCR0B) && defined(TCCR0C)
        _MAKE_TCCR_ABC(0);
#elif defined(TCCR0A) && defined(TCCR0B)
        _MAKE_TCCR_AB(0);
#elif defined(TCCR0)
        _MAKE_TCCR(0);
#endif

#if defined(TCCR1A) && defined(TCCR1B) && defined(TCCR1C)
        _MAKE_TCCR_ABC(1);
#elif defined(TCCR1A) && defined(TCCR1B)
        _MAKE_TCCR_AB(1);
#elif defined(TCCR1)
        _MAKE_TCCR(1);
#endif

#if defined(TCCR2A) && defined(TCCR2B) && defined(TCCR2C)
        _MAKE_TCCR_ABC(2);
#elif defined(TCCR2A) && defined(TCCR2B)
        _MAKE_TCCR_AB(2);
#elif defined(TCCR2)
        _MAKE_TCCR(2);
#endif

#if defined(TCCR3A) && defined(TCCR3B) && defined(TCCR3C)
        _MAKE_TCCR_ABC(3);
#elif defined(TCCR3A) && defined(TCCR3B)
        _MAKE_TCCR_AB(3);
#elif defined(TCCR3)
        _MAKE_TCCR(3);
#endif

#if defined(TCCR4A) && defined(TCCR4B) && defined(TCCR4C)
        _MAKE_TCCR_ABC(4);
#elif defined(TCCR4A) && defined(TCCR4B)
        _MAKE_TCCR_AB(4);
#elif defined(TCCR4)
        _MAKE_TCCR(4);
#endif

#if defined(TCCR5A) && defined(TCCR5B) && defined(TCCR5C)
        _MAKE_TCCR_ABC(5);
#elif defined(TCCR5A) && defined(TCCR5B)
        _MAKE_TCCR_AB(5);
#elif defined(TCCR5)
        _MAKE_TCCR(5);
#endif

    }
}

#endif
