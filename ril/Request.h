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

#ifndef __REQUEST__H__
#define __REQUEST__H__

#include <telephony/ril.h>
#include <vector>

class RequestQueue;

class Request {
public:
    enum State {
        Queued,
        Executing,
        Finished
    };

    Request(int code, const std::vector<char> &data, RIL_Token token, RequestQueue *queue);

    inline State state() const { return m_state; }
    inline int code() const { return m_code; }
    inline const std::vector<char> &data() const { return m_data; }
    inline RIL_Token token() const { return m_token; }
    inline RIL_Errno errno() const { return m_errno; }
    inline const std::vector<char> &reply() const { return m_reply; }

    void complete(RIL_Errno e, const std::vector<char> &reply = std::vector<char>());

private:
    friend class RequestQueue;

    void markAsExecuting();
    void markAsFinished();

private:
    State m_state;
    int m_code;
    std::vector<char> m_data;
    RIL_Token m_token;
    RequestQueue *m_queue;
    RIL_Errno m_errno;
    std::vector<char> m_reply;
};

#endif

