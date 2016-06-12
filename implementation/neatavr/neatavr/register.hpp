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

#ifndef NEATAVR_REGISTER_HPP
#define NEATAVR_REGISTER_HPP

#include "core.hpp"

namespace NeatAVR {
    template<typename _Register, uint8 _number> class Bit {
    public:
        typedef _Register Register;

        static constexpr uint8 number = _number;

        static inline uint8 get() ALWAYS_INLINE {
            return Register::get_bit(number);
        }
        static inline void set() ALWAYS_INLINE {
            Register::set_bit(number);
        }
        static inline void set(uint8 value) ALWAYS_INLINE {
            Register::set_bit(number, value);
        }
        static inline void clear() ALWAYS_INLINE {
            Register::clear_bit(number);
        }
        static inline void toggle() ALWAYS_INLINE {
            Register::toggle_bit(number);
        }

        static inline void on() ALWAYS_INLINE {
            set();
        }
        static inline void off() ALWAYS_INLINE {
            clear();
        }

        static inline void enable() ALWAYS_INLINE {
            set();
        }
        static inline void disable() ALWAYS_INLINE {
            clear();
        }
    };

    template<typename _Type, uint16 _address> class Register {
    public:
        typedef _Type Type;

        static constexpr uint16 address = _address;
        static constexpr volatile Type* pointer = reinterpret_cast<volatile Type*>(address);

        static inline uint8 get_bit(uint8 number) ALWAYS_INLINE {
            return (*pointer >> number) & 1;
        }
        static inline void set_bit(uint8 number) ALWAYS_INLINE {
            *pointer |= 1 << number;
        }
        static inline void set_bit(uint8 number, uint8 value) ALWAYS_INLINE {
            if (value) {
                set_bit(number);
            } else {
                clear_bit(number);
            }
        }
        static inline void clear_bit(uint8 number) ALWAYS_INLINE {
            *pointer &= ~(1 << number);
        }
        static inline void toggle_bit(uint8 number) ALWAYS_INLINE {
            *pointer ^= 1 << number;
        }

        static inline uint8 bit(uint8 number) ALWAYS_INLINE {
            return get_bit(number);
        }
        static inline void bit(uint8 number, uint8 value) ALWAYS_INLINE {
            set_bit(number, value);
        }

    };

    template<uint16 address> class Register8 : public Register<uint8, address> {
    public:
        using Register<uint8, address>::pointer;

        static constexpr uint8 size = 8;

        static inline uint8 get() ALWAYS_INLINE {
            return *pointer;
        }
        static inline uint8 get_low() ALWAYS_INLINE {
            return *pointer;
        }
        static inline uint8 get_high() ALWAYS_INLINE {
            return 0;
        }


        static inline void set(uint8 value) ALWAYS_INLINE {
            *pointer = value;
        }
        static inline void set_low(uint8 value) ALWAYS_INLINE {
            *pointer = value;
        }
        static inline void set_high(uint8 value) ALWAYS_INLINE {

        }

        static inline uint8 bits(uint8 start, uint8 length) ALWAYS_INLINE {
            return (get() >> start) & ((1 << length) - 1);
        }
        static inline void bits(uint8 start, uint8 length, uint8 value) ALWAYS_INLINE {
            set((get() & (0xff & (~(((1 << length) - 1) << start)))) | (value << start));
        }

        typedef Bit<Register8, 0> Bit0;
        typedef Bit<Register8, 1> Bit1;
        typedef Bit<Register8, 2> Bit2;
        typedef Bit<Register8, 3> Bit3;
        typedef Bit<Register8, 4> Bit4;
        typedef Bit<Register8, 5> Bit5;
        typedef Bit<Register8, 6> Bit6;
        typedef Bit<Register8, 7> Bit7;
    };

    template<uint16 address> class Register16 : public Register<uint16, address> {
    public:
        using Register<uint16, address>::pointer;

        static constexpr uint8 size = 16;

        static constexpr volatile uint8* low = reinterpret_cast<volatile uint8*>(address);
        static constexpr volatile uint8* high = reinterpret_cast<volatile uint8*>(address + 1);

        static inline uint16 get() ALWAYS_INLINE {
            uint16 temp;
            SYNCHRONIZED(SYNCHRONIZED_RESTORE) {
                 temp = *pointer;
            }
            return temp;
        }
        static inline uint8 get_low() ALWAYS_INLINE {
            return *low;
        }
        static inline uint8 get_high() ALWAYS_INLINE {
            return *high;
        }

        static inline void set(uint16 value) ALWAYS_INLINE {
            SYNCHRONIZED(SYNCHRONIZED_RESTORE) {
                *pointer = value;
            }
        }
        static inline void set_low(uint8 value) ALWAYS_INLINE {
            *low = value;
        }
        static inline void set_high(uint8 value) ALWAYS_INLINE {
            *high = value;
        }

        static inline uint16 bits(uint8 start, uint8 length) ALWAYS_INLINE {
            return (get() >> start) & ((1 << length) - 1);
        }
        static inline void bits(uint8 start, uint8 length, uint16 value) ALWAYS_INLINE {
            set((get() & (0xffff & (~(((1 << length) - 1) << start)))) | (value << start));
        }

        typedef Bit<Register16, 0> Bit0;
        typedef Bit<Register16, 1> Bit1;
        typedef Bit<Register16, 2> Bit2;
        typedef Bit<Register16, 3> Bit3;
        typedef Bit<Register16, 4> Bit4;
        typedef Bit<Register16, 5> Bit5;
        typedef Bit<Register16, 6> Bit6;
        typedef Bit<Register16, 7> Bit7;
        typedef Bit<Register16, 8> Bit8;
        typedef Bit<Register16, 9> Bit9;
        typedef Bit<Register16, 10> Bit10;
        typedef Bit<Register16, 11> Bit11;
        typedef Bit<Register16, 12> Bit12;
        typedef Bit<Register16, 13> Bit13;
        typedef Bit<Register16, 14> Bit14;
        typedef Bit<Register16, 15> Bit15;
    };

    template<typename _Register, int _start, int _size> class Slice {
    public:
        typedef _Register Register;

        static constexpr uint8 size = _size;
        static constexpr uint8 start = _start;

        static inline void set(uint16 value) ALWAYS_INLINE {
            Register::bits(start, size, value);
        }
        static inline uint16 get() ALWAYS_INLINE {
            return Register::bits(start, size);
        }
    };

    template<typename _Slice1, typename _Slice2> class Concat {
    public:
        typedef _Slice1 Slice1;
        typedef _Slice2 Slice2;

        static constexpr uint8 size = Slice1::size + Slice2::size;

        static inline void set(uint16 value) ALWAYS_INLINE {
            Slice1::set(value >> Slice2::size);
            Slice2::set(value & ((1 << Slice2::size) - 1));
        }
        static inline uint16 get() ALWAYS_INLINE {
            return (Slice1::get() << Slice2::size) | Slice1::get();
        }
    };
}

#endif
