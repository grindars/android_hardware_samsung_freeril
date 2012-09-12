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

struct stat;

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

        virtual bool getFirmware(FirmwareType type, std::vector<unsigned char> &data) = 0;

        virtual int readNVData(off_t offset, std::vector<unsigned char> &data) = 0;
        virtual int writeNVData(off_t offset, const std::vector<unsigned char> &data) = 0;
        virtual void backupNVData(bool full = false) = 0;

        virtual int readFile(int fd, std::vector<unsigned char> &data) = 0;
        virtual int writeFile(int fd, const std::vector<unsigned char> &data) = 0;
        virtual int lseekFile(int fd, int offset, int whence) = 0;
        virtual int closeFile(int fd) = 0;
        virtual int renameFile(const std::string &oldName, const std::string &newName) = 0;
        virtual int getFileInfoByHandle(int fd, struct stat *statbuf) = 0;
        virtual int deleteFile(const std::string &filename) = 0;

        virtual int makeDirectory(const std::string &filename) = 0;
        virtual int deleteDirectory(const std::string &filename) = 0;
        virtual int openDirectory(const std::string &filename) = 0;
        virtual int readDirectory(int fd, std::string &filename) = 0;
        virtual int closeDirectory(int fd) = 0;

        virtual int openFile(const std::string &filename, int flags) = 0;
        virtual int ftruncateFile(int fd, unsigned int size) = 0;

    };
}

#endif
