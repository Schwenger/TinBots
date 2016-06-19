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

#ifndef NEATAVR_ATTINY2313_HPP
#define NEATAVR_ATTINY2313_HPP

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

    typedef Channel<Timer0ChannelAMode, Registers::OutputCompare0A, Registers::Timsk::Bit0, Registers::Tccr0::B::Bit7> Timer0ChannelA;
    typedef Channel<Timer0ChannelBMode, Registers::OutputCompare0B, Registers::Timsk::Bit2, Registers::Tccr0::B::Bit6> Timer0ChannelB;

    typedef TwoChannels<
            CounterMixin<
                    TimerWaveGeneration<
                            TimerBase<Timer0Prescaler, Registers::Timsk::Bit1>,
                            Timer0WaveGeneration>,
                    Registers::Tcnt0>,
            Timer0ChannelA, Timer0ChannelB> Timer0;

    typedef Timer0 Timer;


    typedef PortA::Pin2 Pin1;
    typedef PortD::Pin0 Pin2;
    typedef PortD::Pin1 Pin3;
    typedef PortA::Pin1 Pin4;
    typedef PortA::Pin0 Pin5;
    typedef PortD::Pin2 Pin6;
    typedef PortD::Pin3 Pin7;
    typedef PortD::Pin4 Pin8;
    typedef PortD::Pin5 Pin9;
    typedef PortD::Pin6 Pin11;
    typedef PortB::Pin0 Pin12;
    typedef PortB::Pin1 Pin13;
    typedef PortB::Pin2 Pin14;
    typedef PortB::Pin3 Pin15;
    typedef PortB::Pin4 Pin16;
    typedef PortB::Pin5 Pin17;
    typedef PortB::Pin6 Pin18;
    typedef PortB::Pin7 Pin19;
}

#endif
