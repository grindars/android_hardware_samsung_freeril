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

#include <stdio.h>
#if defined(MESSAGE_INSPECTION)
#include "MessageInspector.h"
#endif

#include "Log.h"
#include "Utilities.h"
#include "IPCSocketHandler.h"
#include "UnsolicitedHandler.h"

using namespace SamsungIPC;

IPCSocketHandler::IPCSocketHandler(IIPCSocket *socket) : SocketHandler(socket) {

}

void IPCSocketHandler::handleMessage(const Message::Header &header,
                                     const void *data) {

    Message *message = NULL;

    message = Message::create(header, data);

    if(message == NULL) {
        Log::debug("Unknown message type:");
        dumpMessage("RX", header, data);
        Log::debug("");

        return;
    }


#if defined(MESSAGE_INSPECTION)
    Log::debug("mseq: %02hhX, aseq: %02hhX, response: %02hhX",
               header.mseq, header.aseq, header.responseType);

    MessageInspector inspector;
    message->accept(&inspector);
#endif

    switch(header.responseType) {
        case Message::IPC_CMD_INDI:
        case Message::IPC_CMD_NOTI:
        {
            UnsolicitedHandler handler;
            message->accept(&handler);

            break;
        }

        case Message::IPC_CMD_RESP:
            Log::debug("Message %p is response for 0x%02hhX", message,
                        header.aseq);

            break;
    }

    delete message;
}

void IPCSocketHandler::dumpMessage(const char *type, const Message::Header &header,
                                   const void *data) {

    Log::debug("%s: %hu bytes, mseq: %02hhX, aseq: %02hhX, %02hhX, %02hhX, %02hhX",
               type, header.length, header.mseq, header.aseq, header.mainCommand,
               header.subCommand, header.responseType);

    if(header.length > sizeof(Message::Header))
        dump(data, header.length - sizeof(Message::Header));
}
