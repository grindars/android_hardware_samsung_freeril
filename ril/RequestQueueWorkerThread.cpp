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

#include "RequestQueueWorkerThread.h"
#include "RequestQueue.h"
#include "Request.h"
#include "ICompletionHandler.h"
#include "IRequestHandler.h"

using namespace SamsungIPC;

RequestQueueWorkerThread::RequestQueueWorkerThread(RequestQueue *queue) :
    m_queue(queue) {

}

int RequestQueueWorkerThread::run() {
    while(1) {
        m_queue->waitForEvent();

        if(m_queue->closeDown())
            break;

        Request *request = m_queue->getReadyRequest();

        if(request == NULL) {
            continue;
        }

        if(request->state() == Request::Finished) {

            const std::vector<char> &data = request->reply();

            m_queue->handler()->completed(request->token(), request->errno(),
                                          &data[0], data.size());

            delete request;
        } else {
            m_queue->exec()->handle(request);
        }
    }

    return 0;
}