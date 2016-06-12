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

#ifndef NEATAVR_FIFO_HPP
#define NEATAVR_FIFO_HPP

#include "core.hpp"

namespace NeatAVR {
    template<typename Type, int max_size> class FIFO {
    private:
        Type items[max_size];
        uint8 read = 0;
        uint8 write = 0;
        uint8 size = 0;

    public:
        inline uint8 is_empty() ALWAYS_INLINE {
            return (this->size == 0) ? 1 : 0;
        }

        inline uint8 is_full() ALWAYS_INLINE {
            return (this->size == max_size) ? 1 : 0;
        }

        inline uint8 available() ALWAYS_INLINE {
            return (this->size > 0) ? 1 : 0;
        }

        void put(Type item) {
            items[this->write] = item;
            this->write++;
            this->size++;
            if (this->write >= max_size) {
                this->write = 0;
            }
        }

        Type get() {
            Type item = items[this->read];
            this->size--;
            this->read++;
            if (this->read >= max_size) {
                this->read = 0;
            }
            return item;
        }
    };
}

#endif
