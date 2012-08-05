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

#ifndef __IPC_SOCKET_HANDLER__H__
#define __IPC_SOCKET_HANDLER__H__

#include <map>
#include <set>
#include <list>

#include "SocketHandler.h"

namespace SamsungIPC {
    class IUnsolicitedReceiver;
    class Message;

    class IPCSocketHandler: public SocketHandler {
    public:
        IPCSocketHandler(IIPCSocket *socket, IUnsolicitedReceiver *unsolicitedHandler);

        void submit(Message *message);

    protected:
        virtual void handleMessage(const Message::Header &header,
                                   const void *data);

    private:
        static void dumpMessage(const char *type, const Message::Header &header,
                                const void *data);

        IUnsolicitedReceiver *m_unsolicitedHandler;
        std::map<uint8_t, Message *> m_messagesInAir;
        std::set<uint8_t> m_freeSequenceNumbers;
        std::list<Message *> m_messageQueue;

        std::set<uint8_t>::iterator m_allocationIterator;
    };
}

#endif
