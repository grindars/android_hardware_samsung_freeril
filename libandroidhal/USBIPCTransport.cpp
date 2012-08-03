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

#include <errno.h>
#include <fcntl.h>
#include <Log.h>

#include "USBIPCTransport.h"
#include "USBIPCSocket.h"
#include "modem_prj.h"

using namespace HAL;
using namespace SamsungIPC;

USBIPCTransport::USBIPCTransport(const std::string &directory, const std::string &ehci_directory) :
    SysfsControlledDevice(ehci_directory), m_directory(directory),
    m_controlFile(NativeFile::open(directory + "link_pm", O_RDWR, 0)),
    m_bootSocket(NULL) {

}

USBIPCTransport::~USBIPCTransport() {

}

IIPCSocket *USBIPCTransport::createSocket(IIPCTransport::Channel channel) {
    USBIPCSocket *socket;

    switch(channel) {
        case IIPCTransport::Boot:
            socket = new USBIPCSocket(m_directory + "umts_boot0", this);

            if(m_bootSocket == NULL)
                m_bootSocket = socket;

            return socket;

        case IIPCTransport::IPC:
            return new USBIPCSocket(m_directory + "umts_ipc0", this);

        case IIPCTransport::Loopback:
            return new USBIPCSocket(m_directory + "umts_loopback0", this);

        case IIPCTransport::RamDump:
            return new USBIPCSocket(m_directory + "umts_ramdump0", this);

        case IIPCTransport::RFS:
            return new USBIPCSocket(m_directory + "umts_rfs0", this);

        default:
            Log::panic("Invalid channel requested");

            return NULL;
    }
}

void USBIPCTransport::setLink(bool up, bool keepModem) {
    int active = up ? 1 : 0, ret;

    if(!keepModem) {
        if(m_bootSocket == NULL) {
            Log::panic("Open boot socket is required for modem power switching due to limitations of kernel mode driver interface.");
        }

        if(active)
            ret = ioctl(m_bootSocket->fd(), IOCTL_MODEM_ON);
        else
            ret = ioctl(m_bootSocket->fd(), IOCTL_MODEM_OFF);

        if(ret == -1)
            Log::panicErrno("ioctl(IOCTL_MODEM_ON/OFF)");
    }

    ret = ioctl(m_controlFile.fd(), IOCTL_LINK_CONTROL_ENABLE, &active);

    if(ret == -1)
        Log::panicErrno("ioctl(IOCTL_LINK_CONTROL_ENABLE)");

    write("ehci_power", active ? "1" : "0");

    ret = ioctl(m_controlFile.fd(), IOCTL_LINK_CONTROL_ACTIVE, &active);

    if(ret == -1)
        Log::panicErrno("ioctl(IOCTL_LINK_CONTROL_ACTIVE)");
}


bool USBIPCTransport::isLinkUp() {
    int ret = ioctl(m_controlFile.fd(), IOCTL_LINK_CONNECTED);

    if(ret == -1)
        Log::panicErrno("ioctl(IOCTL_LINK_CONNECTED)");

    return ret != 0;
}

bool USBIPCTransport::isWokenUp() {
    int ret = ioctl(m_controlFile.fd(), IOCTL_LINK_GET_HOSTWAKE);

    if(ret == -1)
        Log::panicErrno("ioctl(IOCTL_LINK_GET_HOSTWAKE)");

    return ret == 0;
}

bool USBIPCTransport::connect(void) {
    return toggleTransport(false);
}

bool USBIPCTransport::redetect(void) {
    return toggleTransport(true);
}

bool USBIPCTransport::toggleTransport(bool keep_modem) {
    if(keep_modem) {
        if(!waitForWakeup(true))
            return false;
    }

    setLink(false, keep_modem);

    if(keep_modem) {
        if(!waitForWakeup(false)) {
            setLink(true, true);

            return false;
        }
    }

    setLink(true, keep_modem);

    int tries = 4;

    while(!isLinkUp()) {
        if(--tries == 0) {
            return false;
        }

        usleep(500000);
    }

    return true;
}

bool USBIPCTransport::waitForWakeup(bool status) {
    int tries = 10;
    while(isWokenUp() != status) {
        if(--tries == 0)
            return false;

        usleep(500000);
    }

    return true;
}

void USBIPCTransport::socketDestroyed(USBIPCSocket *socket) {
    if(m_bootSocket == socket)
        m_bootSocket = NULL;
}
