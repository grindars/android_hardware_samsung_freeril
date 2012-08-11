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
#include "RIL.h"

using namespace SamsungIPC;

Request::Request(int code, const void *data, size_t data_size,
                 RIL_Token token, RIL *handler) :
    m_code(code), m_data(data), m_data_size(data_size), m_token(token),
    m_handler(handler) {

}


void Request::complete(RIL_Errno e, const void *response, size_t responselen) {
    m_handler->complete(m_token, e, response, responselen);

    delete this;
}
