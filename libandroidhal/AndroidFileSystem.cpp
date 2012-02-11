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
#include <memory>
#include <errno.h>

#include "AndroidFileSystem.h"
#include "NativeFile.h"
#include "CStyleException.h"

#define NVDATA_SIZE (2 * 1024 * 1024)

HAL::AndroidFileSystem::AndroidFileSystem(const std::string &firmware,
                                          const std::string &nvdata) :
    m_firmware(firmware), m_nvdata(nvdata) {

}

std::string HAL::AndroidFileSystem::getFirmware(SamsungIPC::IFileSystem::FirmwareType type) {
    unsigned int offset, length;

    switch(type) {
    case SamsungIPC::IFileSystem::PSI:
        offset = 0;
        length = 61440;

        break;

    case SamsungIPC::IFileSystem::EBL:
        offset = 61440;
        length = 102400;

        break;

    case SamsungIPC::IFileSystem::SecureImage:
        offset = 10483712;
        length = 2048;

        break;

    case SamsungIPC::IFileSystem::Firmware:
        offset = 163840;
        length = 10321920;

        break;

    case SamsungIPC::IFileSystem::DefaultNVData:
        offset = 10485760;
        length = NVDATA_SIZE;

        break;

    default:
        offset = 0;
        length = 0;

        break;
    }

    HAL::NativeFile file = HAL::NativeFile::open(m_firmware, O_RDONLY);

    file.seek(offset, SEEK_SET);

    std::auto_ptr<char> data(new char[length]);
    ssize_t bytes = file.read(data.get(), length);
    if((unsigned int) bytes != length) {
        errno = EIO;

        HAL::throwErrno();
    }

    std::string out;
    out.assign(data.get(), length);
    return out;
}

// TODO: MD5 checking.

std::string HAL::AndroidFileSystem::readNVData() {
    HAL::NativeFile file = HAL::NativeFile::open(m_nvdata, O_RDONLY);

    std::auto_ptr<char> data(new char[NVDATA_SIZE]);
    ssize_t bytes = file.read(data.get(), NVDATA_SIZE);
    if(bytes != NVDATA_SIZE) {
        errno = EIO;

        HAL::throwErrno();
    }

    std::string out;
    out.assign(data.get(), NVDATA_SIZE);
    return out;
}

void HAL::AndroidFileSystem::writeNVData(const std::string &data) {
    HAL::NativeFile file = HAL::NativeFile::open(m_nvdata, O_WRONLY | O_TRUNC | O_CREAT, 0600);

    ssize_t bytes = file.write(data.data(), data.length());

    if(bytes != (ssize_t) data.length()) {
        errno = EIO;

        HAL::throwErrno();
    }
}

