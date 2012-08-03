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

#ifndef __SAMSUNGIPC__IFILESYSTEM__H__
#define __SAMSUNGIPC__IFILESYSTEM__H__

#include <vector>

namespace SamsungIPC {
    class IFileSystem {
    public:
        enum FirmwareType {
            PSI           = 1,
            EBL           = 2,
            SecureImage   = 3,
            Firmware      = 4,
            DefaultNVData = 5
        };

        virtual ~IFileSystem() {}

        virtual bool getFirmware(FirmwareType type, std::vector<char> &data) = 0;

        virtual bool readNVData(std::vector<char> &data) = 0;
        virtual bool writeNVData(const std::vector<char> &data) = 0;
    };
}

#endif
