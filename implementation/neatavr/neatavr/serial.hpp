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

#ifndef NEATAVR_SERIAL_HPP
#define NEATAVR_SERIAL_HPP

#include "core.hpp"

#define _baud_to_ubrr(baud) (((F_CPU + (baud) * 8) / (16 * (baud))) - 1)
#define _baud_to_ubrr_2x(baud) (((F_CPU + (baud) * 4) / (8 * (baud))) - 1)
#define _ubrr_to_baud(ubrr) (F_CPU / (16 * (ubrr) + 8))
#define _ubrr_to_baud_2x(ubrr) (F_CPU / (8 * (ubrr) + 4))
#define _baud_error(baud, ubrr) ((baud * 1000) / _ubrr_to_baud(ubrr))
#define _baud_error_2x(baud, ubrr) ((baud * 1000) / _ubrr_to_baud_2x(ubrr))

#ifdef URSEL
#define URSEL_OR (1 << URSEL)
#else
#define URSEL_OR 0
#endif

#undef U2X

#define _MAKE_SERIAL(NUMBER)                                                                                \
    typedef Register8<_SFR_ADDR(UDR##NUMBER)> _SerialData##NUMBER;                                          \
    class _SerialControlStatus##NUMBER {                                                                    \
    public:                                                                                                 \
        typedef Register8<_SFR_ADDR(UCSR##NUMBER##A)> A;                                                    \
        typedef Register8<_SFR_ADDR(UCSR##NUMBER##B)> B;                                                    \
        typedef Register8<_SFR_ADDR(UCSR##NUMBER##C)> C;                                                    \
    };                                                                                                      \
    typedef Register16<_SFR_ADDR(UBRR##NUMBER)> _SerialBaud##NUMBER;                                        \
                                                                                                            \
    typedef _Serial<_SerialData##NUMBER, _SerialControlStatus##NUMBER, _SerialBaud##NUMBER> Serial##NUMBER;

namespace NeatAVR {
    template<typename _Data, typename _ControlStatus, typename _BaudRate> class _Serial {
    public:
        typedef _ControlStatus ControlStatus;
        typedef _BaudRate BaudRate;

        class Data : public _Data {
        public:
            typedef typename ControlStatus::B::Bit5 Interrupt;
            typedef typename ControlStatus::A::Bit5 Empty;
        };

        class U2X : public ControlStatus::A::Bit1 {
        public:
            static constexpr uint8 DISABLED = 0;
            static constexpr uint8 ENABLED = 1;
            static constexpr uint8 AUTO = 2;
        };

        class TX : public ControlStatus::B::Bit3 {
        public:
            typedef typename ControlStatus::B::Bit6 Interrupt;
            typedef typename ControlStatus::A::Bit6 Complete;
        };
        class RX : public ControlStatus::B::Bit4 {
        public:
            typedef typename ControlStatus::B::Bit7 Interrupt;
            typedef typename ControlStatus::A::Bit7 Complete;
        };



        class Error : public ControlStatus::A {
        public:
            typedef Bit<typename ControlStatus::A, 4> Frame;
            typedef Bit<typename ControlStatus::A, 3> Overrun;
            typedef Bit<typename ControlStatus::A, 2> Parity;
        };

        class Mode {
        public:
            static constexpr uint8 ASYNC = 0b00;
            static constexpr uint8 SYNC = 0b01;

            static inline uint8 get() ALWAYS_INLINE {
                return ControlStatus::C::get() >> 6;
            }
            static inline void set(uint8 value) ALWAYS_INLINE {
                ControlStatus::C::set((ControlStatus::C::get() & 0b00111111) | (value << 6) | URSEL_OR);
            }
        };

        class Parity {
        public:
            static constexpr uint8 DISABLED = 0b00;
            static constexpr uint8 EVEN = 0b10;
            static constexpr uint8 ODD = 0b11;

            static inline uint8 get() ALWAYS_INLINE {
                return (ControlStatus::C::get() >> 4) & 0b11;
            }
            static inline void set(uint8_t value) ALWAYS_INLINE {
                ControlStatus::C::set((ControlStatus::C::get() & 0b11001111) | (value << 4) | URSEL_OR);
            }
        };

        class StopBit {
            static constexpr uint8 ONE = 0;
            static constexpr uint8 TWO = 1;

            static inline uint8 get() ALWAYS_INLINE {
                return ControlStatus::C::bit(4);
            }
            static inline void set(uint8 value) ALWAYS_INLINE {
                ControlStatus::C::set((ControlStatus::C::get() & 0b11110111) | ((value & 1) << 3) | URSEL_OR);
            }
        };

        class CharSize {
        public:
            static constexpr uint8 SIZE_5 = 0b000;
            static constexpr uint8 SIZE_6 = 0b001;
            static constexpr uint8 SIZE_7 = 0b010;
            static constexpr uint8 SIZE_8 = 0b011;
            static constexpr uint8 SIZE_9 = 0b111;

            static inline uint8_t get() ALWAYS_INLINE {
                return (ControlStatus::B::bit(2) << 2) | ((ControlStatus::C::get() >> 1) & 0b11);
            }
            static inline void set(uint8_t value) ALWAYS_INLINE {
                ControlStatus::B::bit(2, (value >> 2) & 1);
                ControlStatus::C::set((ControlStatus::C::get() & 0b11111001) | ((value & 0b11) << 1) | URSEL_OR);
            }
        };

        static inline void init(unsigned long baudrate=9600, uint8_t size=CharSize::SIZE_8, uint8_t u2x=U2X::AUTO, uint8 max_error=20) ALWAYS_INLINE {
            uint16 ubrr = _baud_to_ubrr(baudrate);
            if (u2x == U2X::AUTO) {
                uint16 error = _baud_error(baudrate, ubrr);
                if (error > (1000 + max_error) || error < (1000 - max_error)) {
                    ubrr = _baud_to_ubrr_2x(baudrate);
                    u2x = U2X::ENABLED;
                } else {
                    u2x = U2X::DISABLED;
                }
            }
            if (u2x == U2X::ENABLED) {
                U2X::enable();
            }
            BaudRate::set(ubrr);
            RX::enable();
            TX::enable();
            CharSize::set(size);
        }

        static inline uint8 rx_complete() ALWAYS_INLINE {
            return RX::Complete::get();
        }
        static inline uint8 tx_complete() ALWAYS_INLINE {
            return TX::Complete::get();
        }
        static inline uint8 data_empty() ALWAYS_INLINE {
            return Data::Empty::get();
        }
        static inline uint8 frame_error() ALWAYS_INLINE {
            return Error::Frame::get();
        }
        static inline uint8 overrun() ALWAYS_INLINE {
            return Error::Overrun::get();
        }
        static inline uint8 parity_error() ALWAYS_INLINE {
            return Error::Parity::get();
        }

        static inline void put(uint8 data) ALWAYS_INLINE {
            Data::set(data);
        }
        static inline void put9(uint16 data) ALWAYS_INLINE {
            Data::set(data & 0xff);
            ControlStatus::B::bit(0, (data >> 8));
        }

        static inline uint8 get() ALWAYS_INLINE {
            return Data::get();
        }
        static inline uint16 get9() ALWAYS_INLINE {
            return Data::get() | (ControlStatus::B::bit(1) << 8);
        }

        static void write(const char *message, uint16 length) {
            uint16 i;
            for (i = 0; i < length; i++) {
                while (!(data_empty()));
                put(*message);
                message++;
            }
        }

        static void print(const char *message) {
            while(*message) {
                while (!(data_empty()));
                put((uint8) *message);
                message++;
            }
        }
        static void printline(const char *message) {
            print(message);
            print("\n");
        }

        static void read(char *buffer, uint16 length) {
            uint16 i;
            for (i = 0; i < length; i++) {
                while (!rx_complete()) { };
                buffer[i] = (char) get();
            }
            buffer[i] = '\0';
        }
        static void read_until(char *buffer, const char delimiter, uint16 max_length = 65535) {
            uint16 i;
            for (i = 0; i < max_length; i++) {
                while (!rx_complete()) { };
                buffer[i] = (char) get();
                if (buffer[i] == delimiter) {
                    break;
                }
            }
            buffer[i] = '\0';
        }
        static void readline(char *buffer, uint16 max_length=65535) {
            read_until(buffer, '\n', max_length);
        }
    };

#ifdef UDR
    #define UDR0 UDR
    #define UCSR0A UCSRA
    #define UCSR0B UCSRB
    #define UCSR0C UCSRC
    #define UBRR0H UBRRH
    #define UBRR0L UBRRL
    #define SERIAL0_RX_INTERRUPT USART_RX_vect
    #define SERIAL0_TX_INTERRUPT USART_TX_vect
    #define SERIAL0_UDR_INTERRUPT USART_UDRE_vect
#endif

#ifdef UDR0
    #ifndef SERIAL0_RX_INTERRUPT
        #define SERIAL0_RX_INTERRUPT USART0_RX_vect
        #define SERIAL0_TX_INTERRUPT USART0_TX_vect
        #define SERIAL0_UDR_INTERRUPT USART0_UDRE_vect
    #endif

    _MAKE_SERIAL(0);

    #define SERIAL_RX_INTERRUPT SERIAL0_RX_INTERRUPT
    #define SERIAL_TX_INTERRUPT SERIAL0_TX_INTERRUPT
    #define SERIAL_UDR_INTERRUPT SERIAL0_UDR_INTERRUPT

    typedef Serial0 Serial;
#endif

#ifdef UDR1
    #define SERIAL1_RX_INTERRUPT USART1_RX_vect
    #define SERIAL1_TX_INTERRUPT USART1_TX_vect
    #define SERIAL1_UDR_INTERRUPT USART1_UDRE_vect
    _MAKE_SERIAL(1);
#endif

#ifdef UDR2
    #define SERIAL2_RX_INTERRUPT USART2_RX_vect
    #define SERIAL2_TX_INTERRUPT USART2_TX_vect
    #define SERIAL2_UDR_INTERRUPT USART2_UDRE_vect
    _MAKE_SERIAL(2);
#endif

#ifdef UDR3
    #define SERIAL3_RX_INTERRUPT USART3_RX_vect
    #define SERIAL3_TX_INTERRUPT USART3_TX_vect
    #define SERIAL3_UDR_INTERRUPT USART2_UDRE_vect
    _MAKE_SERIAL(3);
#endif

#endif
}