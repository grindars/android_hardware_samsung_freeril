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
#include "MessageFactory.h"
#include "DataStream.h"
#include <IUnsolicitedReceiver.h>

using namespace SamsungIPC;

IPCSocketHandler::IPCSocketHandler(IIPCSocket *socket, IUnsolicitedReceiver *unsolicitedHandler) : SocketHandler(socket),
    m_unsolicitedHandler(unsolicitedHandler) {

}

void IPCSocketHandler::handleMessage(const Message::Header &header,
                                     const void *data) {

    Message *message = NULL;

    message = Messages::Factory::create(header.mainCommand, header.subCommand);

    if(message == NULL) {
        Log::debug("Unknown message type:");
        dumpMessage("RX", header, data);

        return;
    }

    std::vector<unsigned char> buffer((unsigned char *) data,
                                      (unsigned char *) data + header.length - sizeof(Message::Header));

    DataStream stream(&buffer, DataStream::Read);
    if(!message->readFromStream(stream) || !stream.atEnd()) {
        Log::error("Message demarshalling failed:");
        dumpMessage("RX", header, data);

        delete message;

        return;
    }

    Log::debug("RX mseq: %02hhX, aseq: %02hhX, response: %02hhX",
               header.mseq, header.aseq, header.responseType);

    Log::debug("Message: %s", message->inspect().c_str());

    switch(header.responseType) {
        case Message::IPC_CMD_INDI:
        case Message::IPC_CMD_NOTI:
        {
            if(!message->deliver(m_unsolicitedHandler)) {
                Log::error("Message %s unexpectly sent with response type %d.",
                           message->inspect().c_str(), header.responseType);
            }

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

void IPCSocketHandler::submit(Message *message) {
    Log::debug("IPCSocketHandler: message %p submitted", message);

    std::vector<unsigned char> data;
    DataStream stream(&data, DataStream::Write);

    Log::debug("Marshalling message");

    if(!message->writeToStream(stream)) {
        Log::error("Message marshalling failed:\n%s", message->inspect().c_str());

        message->complete(NULL);
        delete message;

        return;
    }

    Log::debug("Marshalled into %d bytes", data.size());

    Message::Header header;
    header.length = sizeof(header) + data.size();
    header.mseq = 0x01;
    header.aseq = 0x00;
    header.mainCommand = message->command();
    header.subCommand = message->subcommand();
    header.responseType = Message::IPC_CMD_EXEC;

    Log::debug("TX mseq: %02hhX, aseq: %02hhX, response: %02hhX",
               header.mseq, header.aseq, header.responseType);

    Log::debug("Message: %s", message->inspect().c_str());

    dumpMessage("TX", header, &data[0]);

    sendMessage(header, &data[0]);
}
