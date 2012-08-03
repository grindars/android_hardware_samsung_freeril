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

#include "SocketHandler.h"
#include "IIPCSocket.h"
#include "Utilities.h"
#include "Log.h"

#include <stdio.h>
#include <stdlib.h>

using namespace SamsungIPC;

SocketHandler::SocketHandler(IIPCSocket *socket) : m_socket(socket),
    m_reassemblyBuf(NULL), m_reassemblyBufSize(IOBufSize), m_reassemblyBufUsed(0) {

    m_reassemblyBuf = (unsigned char *) malloc(IOBufSize);
    if(m_reassemblyBuf == NULL)
        Log::panicErrno("malloc");
}

SocketHandler::~SocketHandler() {
    delete[] m_buf;
    delete m_socket;

    for(std::list<send_record>::iterator it = m_sendQueue.begin(), end = m_sendQueue.end(); it != end; it++) {
        delete[] (*it).data;
    }

    free(m_reassemblyBuf);
}

int SocketHandler::fd() const {
    return m_socket->fd();
}

bool SocketHandler::wantRead() const {
    return true;
}

bool SocketHandler::wantWrite() const {
    return !m_sendQueue.empty();
}

void SocketHandler::readable() {
    ssize_t size = m_socket->recv(m_reassemblyBuf + m_reassemblyBufUsed, m_reassemblyBufSize - m_reassemblyBufUsed, -1);

    if(size == 0)
        handleEOF();

    m_reassemblyBufUsed += size;

    while(m_reassemblyBufUsed >= sizeof(Message::Header)) {
        Message::Header *header = (Message::Header *) m_reassemblyBuf;

        if(m_reassemblyBufUsed < header->length)
            break;

        handleMessage(*header, header + 1);

        size_t newUsed = m_reassemblyBufUsed - header->length;
        memcpy(header, (unsigned char *) header + header->length, newUsed);
        m_reassemblyBufUsed = newUsed;
    }

    if(m_reassemblyBufSize - m_reassemblyBufUsed != IOBufSize) {
        size_t newSize = m_reassemblyBufUsed + IOBufSize;
        unsigned char *newBuf = (unsigned char *) realloc(m_reassemblyBuf, newSize);
        if(newBuf == NULL)
            Log::panicErrno("realloc");

        m_reassemblyBufSize = newSize;
        m_reassemblyBuf = newBuf;
    }
}

void SocketHandler::writable(void) {
    if(m_sendQueue.size() != 0) {
        send_record &record = m_sendQueue.front();

        size_t size = m_socket->send(record.data, record.size);

        if(size != record.size)
            handleIOError();

        delete[] record.data;
        m_sendQueue.pop_front();
    }
}

void SocketHandler::sendMessage(const Message::Header &header,
                                const void *data) {

    unsigned char *buf = new unsigned char[header.length];

    memcpy(buf, &header, sizeof(Message::Header));
    memcpy(buf + sizeof(Message::Header), data, header.length -
                                                sizeof(Message::Header));

    send_record record;
    record.data = buf;
    record.size = header.length;

    m_sendQueue.push_back(record);
}

void SocketHandler::handleEOF() {
    Log::panic("End of stream");
}

void SocketHandler::handleIOError() {
    Log::panic("Input/output error");
}