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

#include <fcntl.h>

#include "NativeFile.h"
#include "CStyleException.h"

RIL::NativeFileData::NativeFileData(int _fd) : fd(_fd), refs(1) {

}

void RIL::NativeFileData::deref() {
    if(--refs == 0) {
        close(fd);
        delete this;
    }
}

void RIL::NativeFileData::ref() {
    refs++;
}

RIL::NativeFile::NativeFile(int fd) : m_data(new NativeFileData(fd)) {

}

RIL::NativeFile::~NativeFile() {
    m_data->deref();
}

RIL::NativeFile::NativeFile(const NativeFile &orig) {
    m_data = orig.m_data;

    m_data->ref();
}

RIL::NativeFile RIL::NativeFile::open(const std::string &file, int mode) {
    int fd = ::open(file.c_str(), mode);

    if(fd == -1)
        throwErrno();

    return NativeFile(fd);
}

ssize_t RIL::NativeFile::read(void *buf, size_t size) {
    ssize_t bytes = ::read(fd(), buf, size);

    if(bytes == -1)
        throwErrno();

    return bytes;
}

ssize_t RIL::NativeFile::write(const void *buf, size_t size) {
    ssize_t bytes = ::write(fd(), buf, size);

    if(bytes == -1)
        throwErrno();

    return bytes;
}
