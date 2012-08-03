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

#include <Log.h>

#include "RequestQueue.h"
#include "Request.h"
#include "ICompletionHandler.h"
#include "IRequestHandler.h"

using namespace SamsungIPC;

RequestQueue::RequestQueue(ICompletionHandler *handler, IRequestHandler *exec) :
    m_handler(handler), m_exec(exec), m_worker(this), m_closeDown(false) {

    m_worker.start();
}

RequestQueue::~RequestQueue() {
    m_closeDown = true;
    m_queueSemaphore.give();

    m_worker.wait();

    for(std::list<Request *>::iterator it = m_queue.begin(); it != m_queue.end(); it++)
        delete *it;
}

void RequestQueue::request(int code, const void *data, size_t data_size, RIL_Token t) {
    Request *request = new Request(code, std::vector<char>((char *)data, (char *) data + data_size), t, this);

    m_queueMutex.lock();
    m_queue.push_back(request);

    if(m_queue.size() == 1)
        m_queueSemaphore.give();

    m_queueMutex.unlock();
}

void RequestQueue::cancel(RIL_Token t) {
    m_queueMutex.lock();

    for(std::list<Request *>::iterator it = m_queue.begin(); it != m_queue.end(); it++) {
        Request *req = *it;

        if(req->token() == t) {
            switch(req->state()) {
                case Request::Queued:
                    it = m_queue.erase(it);

                    m_queueMutex.unlock();

                    delete req;

                    m_handler->completed(t, RIL_E_CANCELLED, NULL, 0);

                    break;

                case Request::Finished:
                case Request::Executing:
                    m_queueMutex.unlock();

                    break;
            }

            return;
        }
    }

    m_queueMutex.unlock();
}

Request *RequestQueue::getReadyRequest() {
    m_queueMutex.lock();

    if(m_completionQueue.size() > 0) {
        Request *request = m_completionQueue.front();
        m_completionQueue.pop_front();

        m_queueMutex.unlock();

        return request;
    }

    if(m_queue.size() > 0) {
        Request *request = m_queue.front();
        if(request->state() != Request::Queued) {
            m_queueMutex.unlock();

            return NULL;
        }

        request->markAsExecuting();

        m_queueMutex.unlock();

        return request;
    }

    m_queueMutex.unlock();

    return NULL;
}

void RequestQueue::completeRequest(Request *request) {
    m_queueMutex.lock();

    if(m_queue.size() == 0 || m_queue.front() != request || request->state() != Request::Executing) {
        m_queueMutex.unlock();

        Log::panic("Completing unexpected request");
    }

    request->markAsFinished();

    m_completionQueue.push_back(request);
    m_queue.pop_front();
    m_queueSemaphore.give();

    if(m_queue.size() > 0)
        m_queueSemaphore.give();

    m_queueMutex.unlock();
}

bool RequestQueue::supports(int code) {
    return m_exec->supports(code);
}
