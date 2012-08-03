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

#include "Request.h"
#include "RequestQueue.h"

using namespace SamsungIPC;

Request::Request(int code, const std::vector<char> &data,
                 RIL_Token token, RequestQueue *queue) :
    m_state(Queued), m_code(code), m_data(data), m_token(token),
    m_queue(queue) {

}

void Request::markAsExecuting() {
    if(m_state != Queued)
        Log::panic("Illegal request transition from state to %d to state %d\n",
                   m_state, Executing);

    m_state = Executing;
}

void Request::markAsFinished() {
    if(m_state != Executing)
        Log::panic("Illegal request transition from state to %d to state %d\n",
                   m_state, Finished);

    m_state = Finished;

}

void Request::complete(RIL_Errno e, const std::vector<char> &reply) {
    m_errno = e;
    m_reply = reply;

    m_queue->completeRequest(this);
}
