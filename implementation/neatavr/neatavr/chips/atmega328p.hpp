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

#ifndef NEATAVR_ATMEGA328P_HPP
#define NEATAVR_ATMEGA328P_HPP

#include "../port.hpp"

namespace NeatAVR {
    typedef PortC::Pin6 Pin1;
    typedef PortD::Pin0 Pin2, ArduinoD0;
    typedef PortD::Pin1 Pin3, ArduinoD1;
    typedef PortD::Pin2 Pin4, ArduinoD2;
    typedef PortD::Pin3 Pin5, ArduinoD3;
    typedef PortD::Pin4 Pin6, ArduinoD4;
    typedef PortB::Pin6 Pin9;
    typedef PortB::Pin7 Pin10;
    typedef PortD::Pin5 Pin11, ArduinoD5;
    typedef PortD::Pin6 Pin12, ArduinoD6;
    typedef PortD::Pin7 Pin13, ArduinoD7;
    typedef PortB::Pin0 Pin14, ArduinoD8;
    typedef PortB::Pin1 Pin15, ArduinoD9;
    typedef PortB::Pin2 Pin16, ArduinoD10;
    typedef PortB::Pin3 Pin17, ArduinoD11;
    typedef PortB::Pin4 Pin18, ArduinoD12;
    typedef PortB::Pin5 Pin19, ArduinoD13;
    typedef PortC::Pin0 Pin23, ArduinoA0;
    typedef PortC::Pin1 Pin24, ArduinoA1;
    typedef PortC::Pin2 Pin25, ArduinoA2;
    typedef PortC::Pin3 Pin26, ArduinoA3;
    typedef PortC::Pin4 Pin27, ArduinoA4;
    typedef PortC::Pin5 Pin28, ArduinoA5;
}

#endif
