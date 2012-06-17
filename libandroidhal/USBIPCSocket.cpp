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
#include <errno.h>

#include "USBIPCTransport.h"
#include "USBIPCSocket.h"
#include "NativeFile.h"
#include "CStyleException.h"

using namespace HAL;

USBIPCSocket::USBIPCSocket(const std::string &name, USBIPCTransport *transport) :
    m_file(new NativeFile(NativeFile::open(name, O_RDWR, 0))),
    m_transport(transport) {
}

USBIPCSocket::~USBIPCSocket() {
    if(m_file)
        delete m_file;

    m_transport->socketDestroyed(this);
}

ssize_t USBIPCSocket::send(const void *buf, size_t size) {
    if(m_file == NULL) {
        errno = EBADF;
        HAL::throwErrno();
    }

    return m_file->write(buf, size);
}

// NOTE: not signal-safe.
ssize_t USBIPCSocket::recv(void *buf, size_t size, int timeout) {
    if(m_file == NULL) {
        errno = EBADF;
        HAL::throwErrno();
    }

    if(timeout != -1) {
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET(m_file->fd(), &read_set);
        struct timeval val;
        val.tv_sec = timeout / 1000;
        val.tv_usec = (timeout % 1000) * 1000;

        int ret = select(m_file->fd() + 1, &read_set, NULL, NULL, &val);

        if(ret == -1)
            HAL::throwErrno();
        else if(ret == 0)
            return 0;
    }

    return m_file->read(buf, size);
}

void USBIPCSocket::close() {
    if(m_file == NULL) {
        errno = EBADF;
        HAL::throwErrno();
    }

    delete m_file;
    m_file = NULL;
}

int USBIPCSocket::fd() const {
    if(m_file == NULL) {
        errno = EBADF;
        HAL::throwErrno();
    }

    return m_file->fd();
}
