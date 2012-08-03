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

#ifndef __REQUEST_QUEUE__H__
#define __REQUEST_QUEUE__H__

#include <telephony/ril.h>

#include <Mutex.h>
#include <Semaphore.h>

#include <list>

#include "RequestQueueWorkerThread.h"

class ICompletionHandler;
class IRequestHandler;

class Request;

class RequestQueue {
public:
    RequestQueue(ICompletionHandler *handler, IRequestHandler *m_exec);
    ~RequestQueue();

    void request(int code, const void *data, size_t data_size, RIL_Token t);
    void cancel(RIL_Token t);
    bool supports(int code);

private:
    friend class RequestQueueWorkerThread;

    inline void waitForEvent() { m_queueSemaphore.take(); }
    inline bool closeDown() const { return m_closeDown; }
    inline ICompletionHandler *handler() const { return m_handler; }
    inline IRequestHandler *exec() const { return m_exec; }

    Request *getReadyRequest();

private:
    friend class Request;

    void completeRequest(Request *request);

private:
    ICompletionHandler *m_handler;
    IRequestHandler *m_exec;
    std::list<Request *> m_queue, m_completionQueue;
    SamsungIPC::Mutex m_queueMutex;
    SamsungIPC::Semaphore m_queueSemaphore;
    RequestQueueWorkerThread m_worker;
    bool m_closeDown;

};

#endif

