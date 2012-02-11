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

#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

#include "PHONETIPCTransport.h"
#include "PHONETIPCSocket.h"
#include "CStyleException.h"

HAL::PHONETIPCTransport::PHONETIPCTransport(const std::string &interface) :
    SysfsControlledDevice("/sys/class/net/" + interface),
    m_interface(interface) {

}

HAL::PHONETIPCTransport::~PHONETIPCTransport() {

}

bool HAL::PHONETIPCTransport::isLinkUp() const {
    try {
        return read("connected") == "1\n";
    } catch(std::exception) {
        return false;
    }
}

bool HAL::PHONETIPCTransport::isUp() const {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, m_interface.c_str(), IFNAMSIZ);

    int sock = socket(AF_PHONET, SOCK_DGRAM, 0);

    if(sock == -1)
        return false;

    if(ioctl(sock, SIOCGIFFLAGS, &ifr) == -1) {
        close(sock);

        return false;
    }

    close(sock);

    return (ifr.ifr_flags & IFF_UP) != 0;
}

void HAL::PHONETIPCTransport::setUp(bool up) {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, m_interface.c_str(), IFNAMSIZ);

    int sock = socket(AF_PHONET, SOCK_DGRAM, 0);

    if(sock == -1)
        HAL::throwErrno();


    int ret = ioctl(sock, SIOCGIFFLAGS, &ifr), save = errno;

    if(ret == -1) {
        close(sock);
        errno = save;
        HAL::throwErrno();
    }

    if(up)
        ifr.ifr_flags |= IFF_UP;
    else
        ifr.ifr_flags &= ~IFF_UP;

    ret = ioctl(sock, SIOCSIFFLAGS, &ifr);
    save = errno;

    close(sock);

    if(ret == -1) {
        errno = save;
        HAL::throwErrno();
    }
}

SamsungIPC::IIPCSocket *HAL::PHONETIPCTransport::createSocket(int obj_id) {
    return new PHONETIPCSocket(m_interface, obj_id);
}
