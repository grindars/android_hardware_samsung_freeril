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

class RIL;

class Request {
public:

    Request(int code, const void *data, size_t data_size, RIL_Token token, RIL *handler);

    inline int code() const { return m_code; }
    inline const void *data() const { return m_data; }
    inline size_t data_size() const { return m_data_size; }

    inline RIL_Token token() const { return m_token; }

    void complete(RIL_Errno e, const void *response = NULL, size_t responselen = 0);


private:
    int m_code;
    const void *m_data;
    size_t m_data_size;
    RIL_Token m_token;
    RIL *m_handler;
};

#endif

