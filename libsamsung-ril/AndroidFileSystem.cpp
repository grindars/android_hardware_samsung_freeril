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
#include <memory>
#include <errno.h>

#include "AndroidFileSystem.h"
#include "NativeFile.h"
#include "CStyleException.h"

RIL::AndroidFileSystem::AndroidFileSystem(const std::string &firmware) : m_firmware(firmware) {

}

std::string RIL::AndroidFileSystem::getFirmware(SamsungIPC::IFileSystem::FirmwareType type) {
    unsigned int offset, length;

    switch(type) {
    case SamsungIPC::IFileSystem::PSI:
        offset = 0;
        length = 61440;

        break;

    default:
        offset = 0;
        length = 0;

        break;
    }

    RIL::NativeFile file = RIL::NativeFile::open(m_firmware, O_RDONLY);

    file.seek(offset, SEEK_SET);

    std::auto_ptr<char> data(new char[length]);
    ssize_t bytes = file.read(data.get(), length);
    if((unsigned int) bytes != length) {
        errno = EIO;

        RIL::throwErrno();
    }

    std::string out;
    out.assign(data.get(), length);
    return out;
}
