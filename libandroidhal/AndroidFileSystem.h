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

#ifndef __ANDROIDHAL__ANDROIDFILESYSTEM__H__
#define __ANDROIDHAL__ANDROIDFILESYSTEM__H__

#include <IFileSystem.h>
#include <string>

namespace HAL {
    class AndroidFileSystem: public SamsungIPC::IFileSystem {
    public:
        AndroidFileSystem(const std::string &firmware,
                          const std::string &nvdata);

        virtual bool getFirmware(SamsungIPC::IFileSystem::FirmwareType type, std::vector<char> &data);

        virtual bool readNVData(std::vector<char> &data);
        virtual bool writeNVData(const std::vector<char> &data);

    private:
        std::string m_firmware, m_nvdata;
    };
}

#endif
