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

#ifndef __RFS_SOCKET_HANDLER__H__
#define __RFS_SOCKET_HANDLER__H__

#include "MessageTypes.h"
#include "SocketHandler.h"

namespace SamsungIPC {
    class RFSSocketHandler: public SocketHandler {
    public:
        RFSSocketHandler(IIPCSocket *socket);

    protected:
        void handleMessage(const Message::RFSHeader &header,
                           const void *data);
        void sendMessage(const Message::RFSHeader &header, const void *data);
        virtual size_t headerSize();
        virtual size_t messageSize(const unsigned char *data);
        virtual void handleReassembledMessage(const unsigned char *data);

    private:
        Message *handleNvRead(Message *msg);
        Message *handleNvWrite(Message *msg);

        enum {
            FirstType = Messages::RFS_NV_READ,
            LastType  = Messages::RFS_NV_WRITE
        };

        static Message *(RFSSocketHandler::*const m_handlers[LastType - FirstType + 1])(Message *message);
    };
}

#endif
