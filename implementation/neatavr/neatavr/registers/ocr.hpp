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

#ifndef NEATAVR_OCR_HPP
#define NEATAVR_OCR_HPP

#include <avr/io.h>

#include "../register.hpp"

namespace NeatAVR {
    namespace Registers {

#if defined(OCR0AH) && defined(OCR0AL)
        typedef Register16<_SFR_ADDR(OCR0A)> Ocr0A, OutputCompare0A;
#elif defined(OCR0A)
        typedef Register8<_SFR_ADDR(OCR0A)> Ocr0A, OutputCompare0A;
#endif

#if defined(OCR0BH) && defined(OCR0BL)
        typedef Register16<_SFR_ADDR(OCR0B)> Ocr0B, OutputCompare0B;
#elif defined(OCR0B)
        typedef Register8<_SFR_ADDR(OCR0B)> Ocr0B, OutputCompare0B;
#endif

#if defined(OCR0CH) && defined(OCR0CL)
        typedef Register16<_SFR_ADDR(OCR0C)> Ocr0C, OutputCompare0C;
#elif defined(OCR0C)
        typedef Register8<_SFR_ADDR(OCR0C)> Ocr0C, OutputCompare0C;
#endif

#if defined(OCR1AH) && defined(OCR1AL)
        typedef Register16<_SFR_ADDR(OCR1A)> Ocr1A, OutputCompare1A;
#elif defined(OCR1A)
        typedef Register8<_SFR_ADDR(OCR1A)> Ocr1A, OutputCompare1A;
#endif

#if defined(OCR1BH) && defined(OCR1BL)
        typedef Register16<_SFR_ADDR(OCR1B)> Ocr1B, OutputCompare1B;
#elif defined(OCR1B)
        typedef Register8<_SFR_ADDR(OCR1B)> Ocr1B, OutputCompare1B;
#endif

#if defined(OCR1CH) && defined(OCR1CL)
        typedef Register16<_SFR_ADDR(OCR1C)> Ocr1C, OutputCompare1C;
#elif defined(OCR1C)
        typedef Register8<_SFR_ADDR(OCR1C)> Ocr1C, OutputCompare1C;
#endif

#if defined(OCR2AH) && defined(OCR2AL)
        typedef Register16<_SFR_ADDR(OCR2A)> Ocr2A, OutputCompare2A;
#elif defined(OCR2A)
        typedef Register8<_SFR_ADDR(OCR2A)> Ocr2A, OutputCompare2A;;
#endif

#if defined(OCR2BH) && defined(OCR2BL)
        typedef Register16<_SFR_ADDR(OCR2B)> Ocr2B, OutputCompare2B;
#elif defined(OCR2B)
        typedef Register8<_SFR_ADDR(OCR2B)> Ocr2B, OutputCompare2B;
#endif

#if defined(OCR2CH) && defined(OCR2CL)
        typedef Register16<_SFR_ADDR(OCR2C)> Ocr2C, OutputCompare2C;
#elif defined(OCR2C)
        typedef Register8<_SFR_ADDR(OCR2C)> Ocr2C, OutputCompare2C;
#endif

#if defined(OCR3AH) && defined(OCR3AL)
        typedef Register16<_SFR_ADDR(OCR3A)> Ocr3A, OutputCompare3A;
#elif defined(OCR3A)
        typedef Register8<_SFR_ADDR(OCR3A)> Ocr3A, OutputCompare3A;
#endif

#if defined(OCR3BH) && defined(OCR3BL)
        typedef Register16<_SFR_ADDR(OCR3B)> Ocr3B, OutputCompare3B;
#elif defined(OCR3B)
        typedef Register8<_SFR_ADDR(OCR3B)> Ocr3B, OutputCompare3B;
#endif

#if defined(OCR3CH) && defined(OCR3CL)
        typedef Register16<_SFR_ADDR(OCR3C)> Ocr3C, OutputCompare3C;
#elif defined(OCR3C)
        typedef Register8<_SFR_ADDR(OCR3C)> Ocr3C, OutputCompare3C;
#endif

#if defined(OCR4AH) && defined(OCR4AL)
        typedef Register16<_SFR_ADDR(OCR4A)> Ocr4A, OutputCompare4A;
#elif defined(OCR4A)
        typedef Register8<_SFR_ADDR(OCR4A)> Ocr4A, OutputCompare4A;
#endif

#if defined(OCR4BH) && defined(OCR4BL)
        typedef Register16<_SFR_ADDR(OCR4B)> Ocr4B, OutputCompare4B;
#elif defined(OCR4B)
        typedef Register8<_SFR_ADDR(OCR4B)> Ocr4B, OutputCompare4B;
#endif

#if defined(OCR4CH) && defined(OCR4CL)
        typedef Register16<_SFR_ADDR(OCR4C)> Ocr4C, OutputCompare4C;
#elif defined(OCR4C)
        typedef Register8<_SFR_ADDR(OCR4C)> Ocr4C, OutputCompare4C;
#endif

#if defined(OCR5AH) && defined(OCR5AL)
        typedef Register16<_SFR_ADDR(OCR5A)> Ocr5A, OutputCompare5A;
#elif defined(OCR5A)
        typedef Register8<_SFR_ADDR(OCR5A)> Ocr5A, OutputCompare5A;
#endif

#if defined(OCR5BH) && defined(OCR5BL)
        typedef Register16<_SFR_ADDR(OCR5B)> Ocr5B, OutputCompare5B;
#elif defined(OCR5B)
        typedef Register8<_SFR_ADDR(OCR5B)> Ocr5B, OutputCompare5B;
#endif

#if defined(OCR5CH) && defined(OCR5CL)
        typedef Register16<_SFR_ADDR(OCR5C)> Ocr5C, OutputCompare5C;
#elif defined(OCR5C)
        typedef Register8<_SFR_ADDR(OCR5C)> Ocr5C, OutputCompare5C;
#endif

    }
}

#endif
