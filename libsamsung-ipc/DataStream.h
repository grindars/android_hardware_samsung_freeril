/*
 * Free RIL implementation for Samsung Android-based smartphones.
 * Copyright (C) 2012  Sergey Gridasov <grindars@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __DATA_STREAM__H__
#define __DATA_STREAM__H__

#include <stdint.h>
#include <setjmp.h>

#include <vector>

namespace SamsungIPC {

    class DataStream {
    public:
        enum Mode {
            Read,
            Write
        };

        DataStream(std::vector<unsigned char> *buffer, Mode mode);

        inline bool atEnd() const { return m_ptr == m_buffer->end(); }

        inline bool caughtExceptionHere() { return setjmp(m_buf) == 1; }

        void readRawData(void *data, size_t size);
        void writeRawData(const void *data, size_t size);

        inline void read(uint8_t &value) { readRawData(&value, sizeof(uint8_t)); }
        inline void write(uint8_t value) { writeRawData(&value, sizeof(uint8_t)); }

        inline void read(uint16_t &value) { readRawData(&value, sizeof(uint16_t)); }
        inline void write(uint16_t value) { writeRawData(&value, sizeof(uint16_t)); }

        inline void read(uint32_t &value) { readRawData(&value, sizeof(uint32_t)); }
        inline void write(uint32_t value) { writeRawData(&value, sizeof(uint32_t)); }

    private:
        inline void throwException() { longjmp(m_buf, 1); }

        std::vector<unsigned char> *m_buffer;
        Mode m_mode;
        std::vector<unsigned char>::iterator m_ptr;
        jmp_buf m_buf;
    };
}

#endif
