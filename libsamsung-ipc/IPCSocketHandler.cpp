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

    for(unsigned char i = 0; i <= 0xFE; i++)
        m_freeSequenceNumbers.insert(i);

    m_allocationIterator = m_freeSequenceNumbers.begin();
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
            if(!message->deliver(m_unsolicitedHandler)) {
                Log::error("Message %s unexpectly sent with response type %d.",
                           message->inspect().c_str(), header.responseType);
            }

            delete message;

            break;

        case Message::IPC_CMD_RESP:
        {
            std::map<uint8_t, Message *>::iterator request = m_messagesInAir.find(header.aseq);

            if(request == m_messagesInAir.end()) {
                Log::error("No message with sequence 0x%02hhX in air", header.aseq);

                delete message;

            } else {
                Message *requestMessage = (*request).second;

                m_freeSequenceNumbers.insert(header.aseq);
                m_messagesInAir.erase(header.aseq);

                if(m_allocationIterator == m_freeSequenceNumbers.end()) {
                    m_allocationIterator = m_freeSequenceNumbers.begin();

                    if(m_messageQueue.size() > 0) {
                        Message *queued = m_messageQueue.front();
                        m_messageQueue.pop_front();

                        submit(queued);
                    }
                }

                requestMessage->complete(message);
                delete requestMessage;

                break;
            }

            break;
        }


        default:
            Log::error("Unexpected response type 0x%02hhX", header.responseType);

            delete message;

            break;
    }

}

void IPCSocketHandler::dumpMessage(const char *type, const Message::Header &header,
                                   const void *data) {

    Log::debug("%s: %hu bytes, mseq: %02hhX, aseq: %02hhX, cmd: %02hhX, %02hhX, resp: %02hhX",
               type, header.length, header.mseq, header.aseq, header.mainCommand,
               header.subCommand, header.responseType);

    if(header.length > sizeof(Message::Header))
        dump(data, header.length - sizeof(Message::Header));
}

void IPCSocketHandler::submit(Message *message) {
    Log::debug("IPCSocketHandler: message %p submitted", message);

    if(m_freeSequenceNumbers.size() == 0) {
        Log::debug("No free sequence numbers - queueing");

        m_messageQueue.push_back(message);

        return;
    }

    std::vector<unsigned char> data;
    DataStream stream(&data, DataStream::Write);

    if(!message->writeToStream(stream)) {
        Log::error("Message marshalling failed:\n%s", message->inspect().c_str());

        message->complete(NULL);
        delete message;

        return;
    }

    std::set<uint8_t>::iterator val = m_allocationIterator++;

    if(m_allocationIterator == m_freeSequenceNumbers.end())
        m_allocationIterator = m_freeSequenceNumbers.begin();

    Message::Header header;
    header.length = sizeof(header) + data.size();
    header.mseq = *val;
    header.aseq = 0x00;
    header.mainCommand = message->command();
    header.subCommand = message->subcommand();
    header.responseType = message->requestType();

    Log::debug("TX mseq: %02hhX, aseq: %02hhX, response: %02hhX",
               header.mseq, header.aseq, header.responseType);
    Log::debug("Message: %s", message->inspect().c_str());

    sendMessage(header, &data[0]);

    if(header.responseType != Message::IPC_CMD_EVENT) {
        m_freeSequenceNumbers.erase(val);
        m_messagesInAir.insert(std::pair<uint8_t, Message *>(header.mseq, message));
    } else {
        delete message;
    }
}


void IPCSocketHandler::sendMessage(const Message::Header &header,
                                const void *data) {

    unsigned char *buf = new unsigned char[header.length];

    memcpy(buf, &header, sizeof(Message::Header));
    memcpy(buf + sizeof(Message::Header), data, header.length -
                                                sizeof(Message::Header));

    // 'Ready for write' event and writev call aren't implemented in kernel driver.

    sendData(buf, header.length);
    delete[] buf;
}

size_t IPCSocketHandler::headerSize() {
    return sizeof(Message::Header);
}

size_t IPCSocketHandler::messageSize(const unsigned char *data) {
    return reinterpret_cast<const Message::Header *>(data)->length;
}

void IPCSocketHandler::handleReassembledMessage(const unsigned char *data) {
    const Message::Header *header = reinterpret_cast<const Message::Header *>(data);

    handleMessage(*header, header + 1);
}

