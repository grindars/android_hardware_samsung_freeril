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

#include <string.h>

#include "DataStream.h"

using namespace SamsungIPC;

DataStream::DataStream(std::vector<unsigned char> *buffer, Mode mode) : m_buffer(buffer),
    m_mode(mode), m_ptr(buffer->begin()) {

}


void DataStream::readRawData(void *data, size_t size) {
    if(m_mode == Write)
        throwException();

    size_t remaining = m_buffer->end() - m_ptr;
    if(remaining < size)
        throwException();

    memcpy(data, (const unsigned char *) m_ptr, size);
    m_ptr += size;
}


void DataStream::writeRawData(const void *data, size_t size) {
    if(m_mode == Read)
        throwException();

    m_buffer->insert(m_ptr, (const char *) data, (const char *) data + size);
    m_ptr = m_buffer->end();
}
