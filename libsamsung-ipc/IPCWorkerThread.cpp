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
#include <unistd.h>

#include "IPCWorkerThread.h"
#include "IIPCTransport.h"
#include "IIPCSocket.h"
#include "SocketHandler.h"
#include "Log.h"
#include "IPCSocketHandler.h"

using namespace SamsungIPC;

IPCWorkerThread::IPCWorkerThread() {
    if(pipe(m_controlPipe) == -1)
        Log::panicErrno("pipe");
}

IPCWorkerThread::~IPCWorkerThread() {
    close(m_controlPipe[0]);
    close(m_controlPipe[1]);
}

int IPCWorkerThread::run() {
    dispatchEvents();

    for(std::list<SocketHandler *>::iterator it = m_handlers.begin(), end = m_handlers.end(); it != end; it++) {
        delete *it;
    }

    return 0;
}

void IPCWorkerThread::addHandler(SocketHandler *handler) {
    m_handlers.push_back(handler);
}

void IPCWorkerThread::removeHandler(SocketHandler *handler) {
    m_handlers.remove(handler);
}

void IPCWorkerThread::dispatchEvents() {
    while(1) {
        int maxfd = 0;
        fd_set read_set, write_set;

        for(std::list<SocketHandler *>::iterator it = m_handlers.begin(), end = m_handlers.end(); it != end; it++) {
            SocketHandler *handler = *it;
            int fd = handler->fd();

            maxfd = std::max<int>(maxfd, fd);

            if(handler->wantRead())
                FD_SET(fd, &read_set);

            if(handler->wantWrite())
                FD_SET(fd, &write_set);
        }

        FD_SET(m_controlPipe[0], &read_set);

        int ret = select(maxfd + 1, &read_set, &write_set, NULL, NULL);

        if(ret == -1)
            Log::panicErrno("select");

        for(std::list<SocketHandler *>::iterator it = m_handlers.begin(), end = m_handlers.end(); it != end; it++) {
            SocketHandler *handler = *it;
            int fd = handler->fd();

            if(FD_ISSET(fd, &read_set))
                handler->readable();

            if(FD_ISSET(fd, &write_set))
                handler->writable();
        }

        if(FD_ISSET(m_controlPipe[0], &read_set)) {
            Message *msg;

            int bytes = read(m_controlPipe[0], &msg, sizeof(msg));
            if(bytes == -1)
                Log::panicErrno("read(controlPipe)");

            if(msg == NULL) {
                Log::info("Shutdown requested");

                break;
            } else {
                static_cast<IPCSocketHandler *>(m_handlers.front())->submit(msg);
            }
        }
    }
}

void IPCWorkerThread::submit(Message *message) {
    int bytes = write(m_controlPipe[1], &message, sizeof(message));

    if(bytes != sizeof(message))
        Log::panicErrno("write(controlPipe)");
}

