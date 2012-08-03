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
#include <Log.h>

#include "AndroidFileSystem.h"
#include "NativeFile.h"

#define NVDATA_SIZE (2 * 1024 * 1024)

using namespace SamsungIPC;

HAL::AndroidFileSystem::AndroidFileSystem(const std::string &firmware,
                                          const std::string &nvdata) :
    m_firmware(firmware), m_nvdata(nvdata) {

}

bool HAL::AndroidFileSystem::getFirmware(SamsungIPC::IFileSystem::FirmwareType type, std::vector<char> &data) {
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
        Log::panic("Invalid firmware type");

        break;
    }

    HAL::NativeFile file = HAL::NativeFile::open(m_firmware, O_RDONLY);

    file.seek(offset, SEEK_SET);

    data.resize(length);

    ssize_t bytes = file.read(&data[0], length);
    if((unsigned int) bytes != length) {
        Log::panic("Short read");
    }

    return true;
}

// TODO: MD5 checking.

bool HAL::AndroidFileSystem::readNVData(std::vector<char> &nvdata) {
    if(access(m_nvdata.c_str(), F_OK) == -1)
        return false;

    HAL::NativeFile file = HAL::NativeFile::open(m_nvdata, O_RDONLY);

    nvdata.resize(NVDATA_SIZE);

    ssize_t bytes = file.read(&nvdata[0], NVDATA_SIZE);
    if(bytes != NVDATA_SIZE)
        Log::panic("Short read");

    return true;
}

bool HAL::AndroidFileSystem::writeNVData(const std::vector<char> &nvdata) {
#if 0
    HAL::NativeFile file = HAL::NativeFile::open(m_nvdata, O_WRONLY | O_TRUNC | O_CREAT, 0600);

    ssize_t bytes = file.write(&nvdata[0], nvdata.size());

    if(bytes != (ssize_t) nvdata.size())
        Log::panic("Short write");
#else
    Log::error("AndroidFileSystem::writeNVData: disabled for safety.");

    return false;
#endif
}

