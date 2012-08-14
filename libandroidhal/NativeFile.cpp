/*
 * Free HAL implementation for Samsung Android-based smartphones.
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
#include <Log.h>

#include "NativeFile.h"

using namespace SamsungIPC;

HAL::NativeFileData::NativeFileData(int _fd) : refs(1), fd(_fd) {

}

void HAL::NativeFileData::deref() {
    if(--refs == 0) {
        close(fd);
        delete this;
    }
}

void HAL::NativeFileData::ref() {
    refs++;
}

HAL::NativeFile::NativeFile(int fd) : m_data(new NativeFileData(fd)) {

}

HAL::NativeFile::~NativeFile() {
    m_data->deref();
}

HAL::NativeFile::NativeFile(const NativeFile &orig) {
    m_data = orig.m_data;

    m_data->ref();
}

HAL::NativeFile HAL::NativeFile::open(const std::string &file, int flags,
                                      int mode) {
    int fd = ::open(file.c_str(), flags, mode);

    if(fd == -1)
        Log::panicErrno("open");

    return NativeFile(fd);
}

ssize_t HAL::NativeFile::read(void *buf, size_t size) {
    ssize_t bytes = ::read(fd(), buf, size);

    if(bytes == -1)
        Log::panicErrno("read");

    return bytes;
}

ssize_t HAL::NativeFile::write(const void *buf, size_t size) {
    ssize_t bytes = ::write(fd(), buf, size);

    if(bytes == -1)
        Log::panicErrno("write");

    return bytes;
}

off_t HAL::NativeFile::seek(off_t offset, int whence) {
    off_t ret = ::lseek(fd(), offset, whence);

    if(ret == -1)
        Log::panicErrno("lseek");

    return ret;
}

ssize_t HAL::NativeFile::pread(void *buf, size_t size, off_t offset) {
    ssize_t bytes = ::pread(fd(), buf, size, offset);

    if(bytes == -1)
        Log::panicErrno("pread");

    return bytes;
}

ssize_t HAL::NativeFile::pwrite(const void *buf, size_t size, off_t offset) {
    ssize_t bytes = ::pwrite(fd(), buf, size, offset);

    if(bytes == -1)
        Log::panicErrno("pread");

    return bytes;
}