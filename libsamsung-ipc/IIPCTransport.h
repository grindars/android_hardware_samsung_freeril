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

#ifndef __SAMSUNGIPC__IIPCTRANSPORT__H__
#define __SAMSUNGIPC__IIPCTRANSPORT__H__

namespace SamsungIPC {
    class IIPCSocket;

    class IIPCTransport {
    public:
        enum Channel {
            Boot,
            IPC,
            Loopback,
            RamDump,
            RFS
        };

        enum ModemState {
            On = 1,
            Off
        };


        virtual ~IIPCTransport() {};

        virtual void connect() = 0;
        virtual void redetect() = 0;

        virtual IIPCSocket *createSocket(Channel channel) = 0;
    };
}

#endif
