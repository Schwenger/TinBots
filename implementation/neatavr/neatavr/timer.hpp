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

#ifndef NEATAVR_TIMER_HPP
#define NEATAVR_TIMER_HPP

#include "core.hpp"

namespace NeatAVR {
    template<typename _PrescalerSlice> class PrescalerWithExternal : public _PrescalerSlice {
    public:
        static constexpr uint8 STOP = 0b000;
        static constexpr uint8 NONE = 0b001;
        static constexpr uint8 DIV_8 = 0b010;
        static constexpr uint8 DIV_64 = 0b011;
        static constexpr uint8 DIV_256 = 0b100;
        static constexpr uint8 DIV_1024 = 0b101;
        static constexpr uint8 EXT_FALLING = 0b110;
        static constexpr uint8 EXT_RISING = 0b111;

        static constexpr uint8 DEFAULT = DIV_64;
    };

    template<typename _PrescalerSlice> class PrescalerWithoutExternal : public _PrescalerSlice {
    public:
        static constexpr uint8 STOP = 0b000;
        static constexpr uint8 NONE = 0b001;
        static constexpr uint8 DIV_8 = 0b010;
        static constexpr uint8 DIV_32 = 0b011;
        static constexpr uint8 DIV_64 = 0b100;
        static constexpr uint8 DIV_128 = 0b101;
        static constexpr uint8 DIV_256 = 0b110;
        static constexpr uint8 DIV_1024 = 0b111;

        static constexpr uint8 DEFAULT = DIV_64;
    };

    template<typename _ModeSlice> class WaveGeneration8 : public _ModeSlice {
    public:
        static constexpr uint8 NORMAL = 0b000;
        static constexpr uint8 PWM_PHASE_CORRECT = 0b01;
        static constexpr uint8 CTC = 0b010;
        static constexpr uint8 FAST_PWM = 0b011;
        static constexpr uint8 PWM_PHASE_CORRECT_OCR = 0b101;
        static constexpr uint8 FAST_PWM_OCR = 0b111;

        static inline void pwm() ALWAYS_INLINE {
            _ModeSlice::set(PWM_PHASE_CORRECT);
        }
    };

    template<typename _ModeSlice> class WaveGeneration16 : public _ModeSlice {
    public:
        static constexpr uint8 NORMAL = 0b000;
        static constexpr uint8 PWM_PHASE_CORRECT_8BIT = 0b001;
        static constexpr uint8 PWM_PHASE_CORRECT_9BIT = 0b010;
        static constexpr uint8 PWM_PHASE_CORRECT_10BIT = 0b011;
        static constexpr uint8 CTC = 0b0100;
        static constexpr uint8 FAST_PWM_8BIT = 0b0101;
        static constexpr uint8 FAST_PWM_9BIT = 0b0110;
        static constexpr uint8 FAST_PWM_10BIT = 0b0111;
        static constexpr uint8 PWM_PHASE_AND_FREQ_CORRECT_ICR = 0b1000;
        static constexpr uint8 PWM_PHASE_AND_FREQ_CORRECT_OCR = 0b1001;
        static constexpr uint8 PWM_PHASE_CORRECT_ICR = 0b1010;
        static constexpr uint8 PWM_PHASE_CORRECT_OCR = 0b1011;
        static constexpr uint8 CTC_ICR = 0b1100;
        static constexpr uint8 FAST_PWM_ICR = 0b1110;
        static constexpr uint8 FAST_PWM_OCR = 0b1111;

        static inline void pwm() ALWAYS_INLINE {
            _ModeSlice::set(PWM_PHASE_CORRECT_8BIT);
        }
    };

    template<typename _ModeSlice, typename _Compare, typename _Interrupt, typename _Force> class Channel {
    public:
        typedef _Interrupt Interrupt;
        typedef _Force Force;
        typedef _Compare Compare;

        class Mode : public _ModeSlice {
        public:
            static constexpr uint8 NORMAL = 0b00;
            static constexpr uint8 TOGGLE = 0b01;
            static constexpr uint8 CLEAR = 0b10;
            static constexpr uint8 SET = 0b11;
        };
    };

    template<typename _Prescaler, typename _Interrupt> class TimerBase {
    public:
        typedef _Prescaler Prescaler;
        typedef _Interrupt Interrupt;

        static constexpr uint8 _DEFAULT_PRESCALER = _Prescaler::DEFAULT;

        static inline void init(uint8 prescaler=_DEFAULT_PRESCALER) ALWAYS_INLINE {
            Prescaler::set(prescaler);
        }

        static inline void start(uint8 prescaler=_DEFAULT_PRESCALER) ALWAYS_INLINE {
            Prescaler::set(prescaler);
        }

        static inline void stop() ALWAYS_INLINE {
            Prescaler::set(Prescaler::STOP);
        }
    };

    template<typename _Base, typename _WaveGeneration> class TimerWaveGeneration : public _Base {
    public:
        typedef _WaveGeneration WaveGeneration;
    };

    template<typename _Base, typename _Channel> class ChannelAMixin : public _Base {
    public:
        typedef _Channel ChannelA;
    };

    template<typename _Base, typename _Channel> class ChannelBMixin : public _Base {
    public:
        typedef _Channel ChannelB;
    };

    template<typename _Base, typename _Channel> class ChannelCMixin : public _Base {
    public:
        typedef _Channel ChannelC;
    };

    template<typename _Base, typename _ChannelA, typename _ChannelB, typename _ChannelC> class ThreeChannels : public _Base {
    public:
        typedef _ChannelA ChannelA;
        typedef _ChannelB ChannelB;
        typedef _ChannelC ChannelC;
    };

    template<typename _Base, typename _ChannelA, typename _ChannelB> class TwoChannels : public _Base {
    public:
        typedef _ChannelA ChannelA;
        typedef _ChannelB ChannelB;
    };

    template<typename _Base, typename _Counter> class CounterMixin : public _Base {
    public:
        typedef _Counter Counter;
    };
}

#endif
