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

#include <unistd.h>

#include "RFSSocketHandler.h"
#include "Log.h"
#include "MessageFactory.h"
#include "DataStream.h"
#include "Messages.h"
#include "Utilities.h"

using namespace SamsungIPC;

RFSSocketHandler::RFSSocketHandler(IIPCSocket *socket) : SocketHandler(socket) {

}

void RFSSocketHandler::handleMessage(const Message::RFSHeader &header,
                                     const void *data) {

    Log::debug("Received RFS message. Size %u, type: %hhu, sequence: %hhu, data: %p", header.length, header.type, header.sequence, data);

    if(header.type < FirstType || header.type > LastType || m_handlers[header.type - FirstType] == 0) {
        Log::error("No handler for RFS message. Type: %hhu", header.type);

        return;
    }

    Message *message = Messages::Factory::create(Messages::CMD_RFS, header.type);

    if(message == NULL) {
        Log::error("Unknown RFS message received. Type: %hhu", header.type);

        return;
    }

    std::vector<unsigned char> buffer((unsigned char *) data,
                                      (unsigned char *) data + header.length - sizeof(Message::RFSHeader));

    DataStream stream(&buffer, DataStream::Read);
    if(!message->readFromStream(stream) || !stream.atEnd()) {
        Log::error("RFS Message demarshalling failed. Type: %hhu", header.type);

        delete message;

        return;
    }

    Log::debug("Received request: %s", message->inspect().c_str());

    Message *response = (this->*m_handlers[header.type - FirstType])(message);
    delete message;

    if(response) {
        std::vector<unsigned char> data;
        DataStream stream(&data, DataStream::Write);

        if(!response->writeToStream(stream)) {
            Log::error("Response marshalling failed. Type: %hhu", header.type);

            delete response;

            return;
        }

        Message::RFSHeader responseHeader;
        responseHeader.length = sizeof(Message::RFSHeader) + data.size();
        responseHeader.type = response->subcommand();
        responseHeader.sequence = header.sequence;

        Log::debug("Sending response: %s", response->inspect().c_str());

        sendMessage(responseHeader, &data[0]);

        delete response;
    }
}

size_t RFSSocketHandler::headerSize() {
    return sizeof(Message::RFSHeader);
}

size_t RFSSocketHandler::messageSize(const unsigned char *data) {
    return reinterpret_cast<const Message::RFSHeader *>(data)->length;
}

void RFSSocketHandler::handleReassembledMessage(const unsigned char *data) {
    const Message::RFSHeader *header = reinterpret_cast<const Message::RFSHeader *>(data);

    handleMessage(*header, header + 1);
}

void RFSSocketHandler::sendMessage(const Message::RFSHeader &header,
                                   const void *data) {

    unsigned char *buf = new unsigned char[header.length];

    memcpy(buf, &header, sizeof(Message::RFSHeader));
    memcpy(buf + sizeof(Message::RFSHeader), data, header.length -
                                                   sizeof(Message::RFSHeader));

    // 'Ready for write' event and writev call aren't implemented in kernel driver.

    sendData(buf, header.length);
    delete[] buf;
}

Message *RFSSocketHandler::handleNvRead(Message *msg) {
    Log::debug("handleNvRead(%p)", msg);

    return NULL;
}

Message *RFSSocketHandler::handleNvWrite(Message *msg) {
    Log::debug("handleNvWrite(%p)", msg);

    Messages::RfsNvWrite *nvWrite = static_cast<Messages::RfsNvWrite *>(msg);

    Log::debug("RFS: Writing %u bytes to NVRAM offset 0x%08X:", nvWrite->bytes(), nvWrite->offset());
    dump(&nvWrite->data()[0], nvWrite->bytes());

    Messages::RfsNvWriteReply *reply = new Messages::RfsNvWriteReply();
    reply->setOffset(nvWrite->offset());
    reply->setBytes(nvWrite->bytes());
    reply->setStatus(0x01);

    return reply;
}

Message *(RFSSocketHandler::*const RFSSocketHandler::m_handlers[LastType - FirstType + 1])(Message *message) = {
    &RFSSocketHandler::handleNvRead,
    &RFSSocketHandler::handleNvWrite
};

