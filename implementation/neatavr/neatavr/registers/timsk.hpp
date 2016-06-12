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

#ifndef NEATAVR_TIMSK_HPP
#define NEATAVR_TIMSK_HPP

#include <avr/io.h>

#include "../register.hpp"

namespace NeatAVR {
    namespace Registers {

#if defined(TIMSK0)
        typedef Register8<_SFR_ADDR(TIMSK0)> Timsk0;
        typedef Timsk0 Timsk;
#elif defined(TIMSK)
        typedef Register8<_SFR_ADDR(TIMSK)> Timsk;
        typedef Timsk Timsk0;
#endif

#if defined(TIMSK1)
        typedef Register8<_SFR_ADDR(TIMSK1)> Timsk1;
#endif

#if defined(TIMSK2)
        typedef Register8<_SFR_ADDR(TIMSK2)> Timsk2;
#endif

#if defined(TIMSK3)
        typedef Register8<_SFR_ADDR(TIMSK3)> Timsk3;
#endif

#if defined(TIMSK4)
        typedef Register8<_SFR_ADDR(TIMSK4)> Timsk4;
#endif

#if defined(TIMSK5)
        typedef Register8<_SFR_ADDR(TIMSK5)> Timsk5;
#endif

    }
}

#endif
