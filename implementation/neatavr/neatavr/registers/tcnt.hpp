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

#ifndef NEATAVR_TCNT_HPP
#define NEATAVR_TCNT_HPP

#include <avr/io.h>

#include "../register.hpp"

namespace NeatAVR {
    namespace Registers {

#if defined(TCNT0H) && defined(TCNT0L)
        typedef Register16<_SFR_ADDR(TCNT0)> Tcnt0;
#elif defined(TCNT0)
        typedef Register8<_SFR_ADDR(TCNT0)> Tcnt0;
#endif

#if defined(TCNT1H) && defined(TCNT1L)
        typedef Register16<_SFR_ADDR(TCNT1)> Tcnt1;
#elif defined(TCNT1)
        typedef Register8<_SFR_ADDR(TCNT1)> Tcnt1;
#endif

#if defined(TCNT2H) && defined(TCNT2L)
        typedef Register16<_SFR_ADDR(TCNT2)> Tcnt2;
#elif defined(TCNT2)
        typedef Register8<_SFR_ADDR(TCNT2)> Tcnt2;
#endif

#if defined(TCNT3H) && defined(TCNT3L)
        typedef Register16<_SFR_ADDR(TCNT3)> Tcnt3;
#elif defined(TCNT3)
        typedef Register8<_SFR_ADDR(TCNT3)> Tcnt3;
#endif

#if defined(TCNT4H) && defined(TCNT4L)
        typedef Register16<_SFR_ADDR(TCNT4)> Tcnt4;
#elif defined(TCNT4)
        typedef Register8<_SFR_ADDR(TCNT4)> Tcnt4;
#endif

#if defined(TCNT5H) && defined(TCNT5L)
        typedef Register16<_SFR_ADDR(TCNT5)> Tcnt5;
#elif defined(TCNT5)
        typedef Register8<_SFR_ADDR(TCNT5)> Tcnt5;
#endif

    }
}

#endif
