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

#ifndef NEATAVR_ATMEGA2560_HPP
#define NEATAVR_ATMEGA2560_HPP

#include <avr/interrupt.h>

#include "../core.hpp"
#include "../timer.hpp"
#include "../port.hpp"
#include "../pwm.hpp"


#define _MAKE_MEGA_TIMER16(NUMBER)                                                                                  \
    typedef PrescalerWithExternal<Slice<Registers::Tccr##NUMBER::B, 0, 3>> Timer##NUMBER##Prescaler;                \
                                                                                                                    \
    typedef WaveGeneration16<Concat<Slice<Registers::Tccr##NUMBER::B, 3, 2>,                                        \
                                    Slice<Registers::Tccr##NUMBER::A, 0, 2>>> Timer##NUMBER##WaveGeneration;        \
                                                                                                                    \
    typedef Slice<Registers::Tccr##NUMBER::A, 6, 2> Timer##NUMBER##ChannelAMode;                                    \
    typedef Slice<Registers::Tccr##NUMBER::A, 4, 2> Timer##NUMBER##ChannelBMode;                                    \
    typedef Slice<Registers::Tccr##NUMBER::A, 2, 2> Timer##NUMBER##ChannelCMode;                                    \
                                                                                                                    \
    typedef Channel<Timer##NUMBER##ChannelAMode, Registers::OutputCompare##NUMBER##A,                               \
                    Registers::Timsk##NUMBER::Bit1, Registers::Tccr##NUMBER::C::Bit7> Timer##NUMBER##ChannelA;      \
    typedef Channel<Timer##NUMBER##ChannelBMode, Registers::OutputCompare##NUMBER##B,                               \
                    Registers::Timsk##NUMBER::Bit2, Registers::Tccr##NUMBER::C::Bit6> Timer##NUMBER##ChannelB;      \
    typedef Channel<Timer##NUMBER##ChannelCMode, Registers::OutputCompare##NUMBER##C,                               \
                    Registers::Timsk##NUMBER::Bit3, Registers::Tccr##NUMBER::C::Bit5> Timer##NUMBER##ChannelC;      \
                                                                                                                    \
    typedef ThreeChannels<                                                                                          \
                CounterMixin<                                                                                       \
                    TimerWaveGeneration<                                                                            \
                        TimerBase<Timer##NUMBER##Prescaler, Registers::Timsk##NUMBER::Bit0>,                        \
                    Timer##NUMBER##WaveGeneration>,                                                                 \
                Registers::Tcnt##NUMBER>,                                                                           \
            Timer##NUMBER##ChannelA, Timer##NUMBER##ChannelB, Timer##NUMBER##ChannelC> Timer##NUMBER;


namespace NeatAVR {
    _MAKE_MEGA_TIMER16(1);
#define TIMER1_OVERFLOW_INTERRUPT TIMER2_OVF_vect

    _MAKE_MEGA_TIMER16(3);
#define TIMER3_OVERFLOW_INTERRUPT TIMER3_OVF_vect

    _MAKE_MEGA_TIMER16(4);
#define TIMER4_OVERFLOW_INTERRUPT TIMER4_OVF_vect

    _MAKE_MEGA_TIMER16(5);
#define TIMER5_OVERFLOW_INTERRUPT TIMER5_OVF_vect


    typedef PrescalerWithExternal<Slice<Registers::Tccr0::B, 0, 3>> Timer0Prescaler;
    typedef WaveGeneration8<Concat<Slice<Registers::Tccr0::B, 3, 1>, Slice<Registers::Tccr0::A, 0, 2>>> Timer0WaveGeneration;

    typedef Slice<Registers::Tccr0::A, 6, 2> Timer0ChannelAMode;
    typedef Slice<Registers::Tccr0::A, 4, 2> Timer0ChannelBMode;

    typedef Channel<Timer0ChannelAMode, Registers::OutputCompare0A, Registers::Timsk0::Bit1, Registers::Tccr0::B::Bit7> Timer0ChannelA;
    typedef Channel<Timer0ChannelBMode, Registers::OutputCompare0B, Registers::Timsk0::Bit2, Registers::Tccr0::B::Bit6> Timer0ChannelB;

    typedef TwoChannels<
                CounterMixin<
                    TimerWaveGeneration<
                        TimerBase<Timer0Prescaler, Registers::Timsk0::Bit0>,
                    Timer0WaveGeneration>,
                Registers::Tcnt0>,
            Timer0ChannelA, Timer0ChannelB> Timer0;

#define TIMER0_OVERFLOW_INTERRUPT TIMER0_OVF_vect


    typedef PrescalerWithoutExternal<Slice<Registers::Tccr2::B, 0, 3>> Timer2Prescaler;
    typedef WaveGeneration8<Concat<Slice<Registers::Tccr2::B, 3, 1>, Slice<Registers::Tccr2::A, 0, 2>>> Timer2WaveGeneration;

    typedef Slice<Registers::Tccr2::A, 6, 2> Timer2ChannelAMode;
    typedef Slice<Registers::Tccr2::A, 4, 2> Timer2ChannelBMode;

    typedef Channel<Timer2ChannelAMode, Registers::OutputCompare2A, Registers::Timsk2::Bit1, Registers::Tccr2::B::Bit7> Timer2ChannelA;
    typedef Channel<Timer2ChannelBMode, Registers::OutputCompare2B, Registers::Timsk2::Bit2, Registers::Tccr2::B::Bit6> Timer2ChannelB;

    typedef TwoChannels<
                CounterMixin<
                    TimerWaveGeneration<
                        TimerBase<Timer2Prescaler, Registers::Timsk2::Bit0>,
                    Timer2WaveGeneration>,
                Registers::Tcnt2>,
            Timer2ChannelA, Timer2ChannelB> Timer2;

#define TIMER2_OVERFLOW_INTERRUPT TIMER2_OVF_vect


    typedef Timer0 Timer;
#define TIMER_OVERFLOW_INTERRUPT TIMER0_OVERFLOW_INTERRUPT

    typedef PWMMixin<PortG::Pin5, Timer0::ChannelB, Timer0> Pin1, ArduinoD4, ArduinoD04;
    typedef PortE::Pin0 Pin2, ArduinoD0, ArduinoD00;
    typedef PortE::Pin1 Pin3, ArduinoD1, ArduinoD01;
    typedef PortE::Pin2 Pin4;
    typedef PWMMixin<PortE::Pin3, Timer3::ChannelA, Timer3> Pin5, ArduinoD5, ArduinoD05;
    typedef PWMMixin<PortE::Pin4, Timer3::ChannelB, Timer3> Pin6, ArduinoD2, ArduinoD02;
    typedef PWMMixin<PortE::Pin5, Timer3::ChannelC, Timer3> Pin7, ArduinoD3, ArduinoD03;
    typedef PortE::Pin6 Pin8;
    typedef PortE::Pin7 Pin9;
    typedef PortH::Pin0 Pin12, ArduinoD17;
    typedef PortH::Pin1 Pin13, ArduinoD16;
    typedef PortH::Pin2 Pin14;
    typedef PWMMixin<PortH::Pin3, Timer4::ChannelA, Timer4> Pin15, ArduinoD6, ArduinoD06;
    typedef PWMMixin<PortH::Pin4, Timer4::ChannelB, Timer4> Pin16, ArduinoD7, ArduinoD07;
    typedef PWMMixin<PortH::Pin5, Timer4::ChannelC, Timer4> Pin17, ArduinoD8, ArduinoD08;
    typedef PWMMixin<PortH::Pin6, Timer2::ChannelB, Timer2> Pin18, ArduinoD9, ArduinoD09;
    typedef PortB::Pin0 Pin19, ArduinoD53;
    typedef PortB::Pin1 Pin20, ArduinoD52;
    typedef PortB::Pin2 Pin21, ArduinoD51;
    typedef PortB::Pin3 Pin22, ArduinoD50;
    typedef PWMMixin<PortB::Pin4, Timer2::ChannelA, Timer2> Pin23, ArduinoD10;
    typedef PWMMixin<PortB::Pin5, Timer1::ChannelA, Timer1> Pin24, ArduinoD11;
    typedef PWMMixin<PortB::Pin6, Timer1::ChannelB, Timer1> Pin25, ArduinoD12;
    typedef PWMMixin<PortB::Pin7, Timer1::ChannelC, Timer1> Pin26, ArduinoD13;
    typedef PortH::Pin7 Pin27;
    typedef PortG::Pin3 Pin28;
    typedef PortG::Pin4 Pin29;
    typedef PortL::Pin0 Pin35, ArduinoD49;
    typedef PortL::Pin1 Pin36, ArduinoD48;
    typedef PortL::Pin2 Pin37, ArduinoD47;
    typedef PWMMixin<PortL::Pin3, Timer5::ChannelA, Timer5> Pin38, ArduinoD46;
    typedef PWMMixin<PortL::Pin4, Timer5::ChannelB, Timer5> Pin39, ArduinoD45;
    typedef PWMMixin<PortL::Pin5, Timer5::ChannelC, Timer5> Pin40, ArduinoD44;
    typedef PortL::Pin6 Pin41, ArduinoD43;
    typedef PortL::Pin7 Pin42, ArduinoD42;
    typedef PortD::Pin0 Pin43, ArduinoD21;
    typedef PortD::Pin1 Pin44, ArduinoD20;
    typedef PortD::Pin2 Pin45, ArduinoD19;
    typedef PortD::Pin3 Pin46, ArduinoD18;
    typedef PortD::Pin4 Pin47;
    typedef PortD::Pin5 Pin48;
    typedef PortD::Pin6 Pin49;
    typedef PortD::Pin7 Pin50, ArduinoD38;
    typedef PortG::Pin0 Pin51, ArduinoD41;
    typedef PortG::Pin1 Pin52, ArduinoD40;
    typedef PortC::Pin0 Pin53, ArduinoD37;
    typedef PortC::Pin1 Pin54, ArduinoD36;
    typedef PortC::Pin2 Pin55, ArduinoD35;
    typedef PortC::Pin3 Pin56, ArduinoD34;
    typedef PortC::Pin4 Pin57, ArduinoD33;
    typedef PortC::Pin5 Pin58, ArduinoD32;
    typedef PortC::Pin6 Pin59, ArduinoD31;
    typedef PortC::Pin7 Pin60, ArduinoD30;
    typedef PortJ::Pin0 Pin63, ArduinoD15;
    typedef PortJ::Pin1 Pin64, ArduinoD14;
    typedef PortJ::Pin2 Pin65;
    typedef PortJ::Pin3 Pin66;
    typedef PortJ::Pin4 Pin67;
    typedef PortJ::Pin5 Pin68;
    typedef PortJ::Pin6 Pin69;
    typedef PortG::Pin2 Pin70, ArduinoD39;
    typedef PortA::Pin7 Pin71, ArduinoD29;
    typedef PortA::Pin6 Pin72, ArduinoD28;
    typedef PortA::Pin5 Pin73, ArduinoD27;
    typedef PortA::Pin4 Pin74, ArduinoD26;
    typedef PortA::Pin3 Pin75, ArduinoD25;
    typedef PortA::Pin2 Pin76, ArduinoD24;
    typedef PortA::Pin1 Pin77, ArduinoD23;
    typedef PortA::Pin0 Pin78, ArduinoD22;
    typedef PortJ::Pin7 Pin79;
    typedef PortK::Pin7 Pin82, ArduinoA15;
    typedef PortK::Pin6 Pin83, ArduinoA14;
    typedef PortK::Pin5 Pin84, ArduinoA13;
    typedef PortK::Pin4 Pin85, ArduinoA12;
    typedef PortK::Pin3 Pin86, ArduinoA11;
    typedef PortK::Pin2 Pin87, ArduinoA10;
    typedef PortK::Pin1 Pin88, ArduinoA9;
    typedef PortK::Pin0 Pin89, ArduinoA8;
    typedef PortF::Pin7 Pin90, ArduinoA7;
    typedef PortF::Pin6 Pin91, ArduinoA6;
    typedef PortF::Pin5 Pin92, ArduinoA5;
    typedef PortF::Pin4 Pin93, ArduinoA4;
    typedef PortF::Pin3 Pin94, ArduinoA3;
    typedef PortF::Pin2 Pin95, ArduinoA2;
    typedef PortF::Pin1 Pin96, ArduinoA1;
    typedef PortF::Pin0 Pin97, ArduinoA0;
}

#endif
