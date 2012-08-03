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
#include <Utilities.h>

#include "RequestHandler.h"
#include "Request.h"

using namespace SamsungIPC;

RequestHandler::RequestHandler(RIL *ril) : m_ril(ril) {

}

void RequestHandler::handle(Request *request) {
    Log::debug("Handling request %d:", request->code());

    dump(&request->data()[0], request->data().size());

    request->complete(RIL_E_RADIO_NOT_AVAILABLE);
}

bool RequestHandler::supports(int request) {
    (void) request;

    return false;
}
