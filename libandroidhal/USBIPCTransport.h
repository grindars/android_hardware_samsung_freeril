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

#ifndef __ANDROIDHAL__USBIPCTRANSPORT__H__
#define __ANDROIDHAL__USBIPCTRANSPORT__H__

#include <IIPCTransport.h>
#include <string>

#include "NativeFile.h"
#include "SysfsControlledDevice.h"

namespace HAL {
    class USBIPCSocket;

    class USBIPCTransport : public SamsungIPC::IIPCTransport,
                            public SysfsControlledDevice {
    public:
        USBIPCTransport(const std::string &directory,
                        const std::string &ehci_directory);
        virtual ~USBIPCTransport();

        virtual void connect();
        virtual void redetect();

        virtual SamsungIPC::IIPCSocket *createSocket(SamsungIPC::IIPCTransport::Channel channel);

    private:
        friend class USBIPCSocket;
        void socketDestroyed(USBIPCSocket *socket);

    private:
        void toggleTransport(bool keepModem);
        void waitForWakeup(bool state);
        void setLink(bool up, bool keepModem);
        bool isLinkUp();
        bool isWokenUp();

        std::string m_directory;
        NativeFile m_controlFile;
        USBIPCSocket *m_bootSocket;
    };
}

#endif
