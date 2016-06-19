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
#include "../timer.hpp"

#define TIMER_OVERLOW_INTERRUPT TIMER0_OVF_vect
#define TIMER_COMPARE_A_INTRRUPT TIMER0_COMPA_vect
#define TIMER_COMPARE_B_INTRRUPT TIMER0_COMPB_vect

namespace NeatAVR {
    typedef PrescalerWithExternal<Slice<Registers::Tccr0::B, 0, 3>> Timer0Prescaler;
    typedef WaveGeneration8<Concat<Slice<Registers::Tccr0::B, 3, 1>, Slice<Registers::Tccr0::A, 0, 2>>> Timer0WaveGeneration;

    typedef Slice<Registers::Tccr0::A, 6, 2> Timer0ChannelAMode;
    typedef Slice<Registers::Tccr0::A, 4, 2> Timer0ChannelBMode;

    typedef Channel<Timer0ChannelAMode, Registers::OutputCompare0A, Registers::Timsk::Bit1, Registers::Tccr0::B::Bit7> Timer0ChannelA;
    typedef Channel<Timer0ChannelBMode, Registers::OutputCompare0B, Registers::Timsk::Bit2, Registers::Tccr0::B::Bit6> Timer0ChannelB;

    typedef TwoChannels<
            CounterMixin<
                    TimerWaveGeneration<
                            TimerBase<Timer0Prescaler, Registers::Timsk::Bit0>,
                            Timer0WaveGeneration>,
                    Registers::Tcnt0>,
            Timer0ChannelA, Timer0ChannelB> Timer0;

    typedef Timer0 Timer;

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
