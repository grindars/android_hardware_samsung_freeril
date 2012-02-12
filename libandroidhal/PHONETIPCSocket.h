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

#ifndef __ANDROIDHAL__PHONETIPCSOCKET__H__
#define __ANDROIDHAL__PHONETIPCSOCKET__H__

#include <IIPCSocket.h>
#include <linux/phonet.h>
#include <string>

namespace HAL {
    class PHONETIPCSocket: public SamsungIPC::IIPCSocket {
        friend class PHONETIPCTransport;

        PHONETIPCSocket(const std::string &interface, int obj_id);
    public:
        virtual ~PHONETIPCSocket();

        virtual void close();
        virtual ssize_t send(const void *buf, size_t size, int object = -1);
        virtual ssize_t recv(void *buf, size_t size, int timeout = 250);

    private:
        struct sockaddr_pn m_addr;
        int m_fd;
    };
}

#endif

