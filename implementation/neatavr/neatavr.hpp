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

#ifndef NEATAVR_NEATAVR_HPP
#define NEATAVR_NEATAVR_HPP

#include "neatavr/core.hpp"
#include "neatavr/register.hpp"
#include "neatavr/port.hpp"

#if defined(__AVR_ATmega2560__)
#include "neatavr/chips/atmega2560.hpp"
#endif

#if defined(__AVR_ATtiny2313__)
#include "neatavr/chips/attiny2313.hpp"
#endif

#if defined(__AVR_ATmega328P__)
#include "neatavr/chips/atmega328p.hpp"
#endif

//#include "neatavr/serial.hpp"
//#include "neatavr/twi.hpp"

#endif
