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

#ifndef __ANDROIDHAL__NATIVEFILE__H__
#define __ANDROIDHAL__NATIVEFILE__H__

#include <string>

namespace HAL {
    struct NativeFileData {
        NativeFileData(int fd);
        void deref();
        void ref();

        int refs;
        int fd;
    };

    class NativeFile {
        NativeFile(int fd);

    public:
        NativeFile(const NativeFile &original);
        ~NativeFile();

        inline int fd() const { return m_data->fd; }
        inline operator int() const { return m_data->fd; }

        static NativeFile open(const std::string &file, int flags, int mode = 0644);

        ssize_t pread(void *buf, size_t size, off_t offset);
        ssize_t read(void *buf, size_t size);
        ssize_t pwrite(const void *buf, size_t size, off_t offset);
        ssize_t write(const void *buf, size_t size);
        off_t seek(off_t offset, int whence);

    private:
        NativeFileData *m_data;
    };
}

#endif
