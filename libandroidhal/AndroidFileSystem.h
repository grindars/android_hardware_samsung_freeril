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
#include <unordered_set>

#include <dirent.h>

namespace std {
    template<> class hash<DIR *> {
    public:
        size_t operator()(DIR *const &dir) const {
            return std::hash<void *>()(dir);
        }
    };
}

namespace HAL {
    class AndroidFileSystem: public SamsungIPC::IFileSystem {
    public:
        AndroidFileSystem(const std::string &firmware,
                          const std::string &nvdata);

        virtual bool getFirmware(SamsungIPC::IFileSystem::FirmwareType type, std::vector<unsigned char> &data);

        virtual int readNVData(off_t offset, std::vector<unsigned char> &data);
        virtual int writeNVData(off_t offset, const std::vector<unsigned char> &data);
        virtual void backupNVData(bool full = false);

        virtual int readFile(int fd, std::vector<unsigned char> &data);
        virtual int writeFile(int fd, const std::vector<unsigned char> &data);
        virtual int lseekFile(int fd, int offset, int whence);
        virtual int closeFile(int fd);
        virtual int renameFile(const std::string &oldName, const std::string &newName);
        virtual int getFileInfoByHandle(int fd, struct stat *statbuf);
        virtual int deleteFile(const std::string &filename);

        virtual int makeDirectory(const std::string &filename);
        virtual int deleteDirectory(const std::string &filename);
        virtual int openDirectory(const std::string &filename);
        virtual int readDirectory(int fd, std::string &filename);
        virtual int closeDirectory(int fd);

        virtual int openFile(const std::string &filename, int flags);
        virtual int ftruncateFile(int fd, unsigned int size);

    private:
        int tryLoadNV(const std::string filename, bool md5_enabled, size_t expected_size);
        void updateMD5();

        static bool calculateFileMD5(int fd, off_t offset, size_t size, unsigned char digest[32]);
        static void calculateMD5(const void *data, size_t size, unsigned char digest[32]);
        static void binaryToHex(const unsigned char *data, size_t size, unsigned char *out);

        int expandAndValidateFilename(const std::string &filename, std::string &expanded);
        static int makePath(const std::string &expanded);
        bool isSafeDescriptor(int fd);
        static int translateReturn(int ret);

        std::string m_firmware, m_efs;
        int m_nvdata;
        bool m_md5_enabled;
        std::tr1::unordered_set<int> m_rfsDescriptors;
        std::tr1::unordered_set<DIR *> m_rfsDirectories;
    };
}

#endif
