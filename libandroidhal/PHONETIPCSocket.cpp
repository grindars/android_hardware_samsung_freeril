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

#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "PHONETIPCSocket.h"
#include "CStyleException.h"

HAL::PHONETIPCSocket::PHONETIPCSocket(const std::string &interface, int obj_id) {
    m_fd = socket(AF_PHONET, SOCK_DGRAM, 0);

    if(m_fd == -1)
        HAL::throwErrno();

    if(setsockopt(m_fd, SOL_SOCKET, SO_BINDTODEVICE, interface.data(), interface.size()) == -1) {
        int save = errno;

        close(m_fd);

        errno = save;

        HAL::throwErrno();
    }

    int reuse = 1;

    if(setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
        int save = errno;

        close(m_fd);

        errno = save;

        HAL::throwErrno();
    }

    memset(&addr, 0, sizeof(struct sockaddr_pn));
    addr.spn_family = AF_PHONET;
    addr.spn_obj = obj_id;

    if(bind(m_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_pn)) == -1) {
        int save = errno;

        close(m_fd);

        errno = save;

        HAL::throwErrno();
    }
}

HAL::PHONETIPCSocket::~PHONETIPCSocket() {
    close(m_fd);
}

ssize_t HAL::PHONETIPCSocket::send(const void *buf, size_t size) {
    ssize_t bytes = sendto(m_fd, buf, size, 0, (struct sockaddr *) &addr, sizeof(struct sockaddr_pn));

    if(bytes == -1)
        HAL::throwErrno();

    return bytes;
}

// NOTE: not signal-safe.
ssize_t HAL::PHONETIPCSocket::recv(void *buf, size_t size, int timeout) {
    if(timeout != -1) {
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET(m_fd, &read_set);
        struct timeval val;
        val.tv_sec = timeout / 1000;
        val.tv_usec = (timeout % 1000) * 1000;

        int ret = select(m_fd + 1, &read_set, NULL, NULL, &val);

        if(ret == -1)
            HAL::throwErrno();
        else if(ret == 0)
            return 0;
    }
    struct sockaddr_pn dummy;
    socklen_t dummy_len = sizeof(struct sockaddr_pn);

    ssize_t bytes = recvfrom(m_fd, buf, size, 0, (struct sockaddr *) &dummy, &dummy_len);

    if(bytes == -1)
        HAL::throwErrno();

    return bytes;
}
