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

#include "SysfsControlledDevice.h"
#include "NativeFile.h"

HAL::SysfsControlledDevice::SysfsControlledDevice(const std::string &path) : m_path(path) {

}

std::string HAL::SysfsControlledDevice::read(const std::string &filename) const {
    std::auto_ptr<char> data(new char[1024]);

    NativeFile file = NativeFile::open(m_path + "/" + filename, O_RDONLY);
    ssize_t bytes = file.read(data.get(), 1024);

    std::string ret;
    ret.assign(data.get(), bytes);
    return ret;
}

void HAL::SysfsControlledDevice::write(const std::string &filename,
                                       const std::string &value) const {

    NativeFile file = NativeFile::open(m_path + "/" + filename, O_WRONLY);

    file.write(value.data(), value.length());
}
