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

#ifndef __SOCKET_HANDLER__H__
#define __SOCKET_HANDLER__H__

#include <string.h>
#include <list>

#include "Message.h"

namespace SamsungIPC {
    class IIPCSocket;

    class SocketHandler {
    public:
        SocketHandler(IIPCSocket *socket);
        virtual ~SocketHandler();

        int fd() const;

        bool wantRead() const;
        bool wantWrite() const;

        void readable();
        void writable();

    protected:
        virtual void handleMessage(const Message::Header &header,
                                   const void *data) = 0;
        virtual void handleEOF();
        virtual void handleIOError();

        void sendMessage(const Message::Header &header, const void *data);

    private:

        enum {
            IOBufSize = 4096
        };

        IIPCSocket *m_socket;
        unsigned char *m_buf;
        unsigned char *m_reassemblyBuf;
        size_t m_reassemblyBufSize, m_reassemblyBufUsed;
    };
}

#endif
