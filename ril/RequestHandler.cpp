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

#include <stdlib.h>
#include <unistd.h>

#include <Log.h>
#include <Utilities.h>

#include "RequestHandler.h"
#include "Request.h"
#include "RIL.h"

using namespace SamsungIPC;

RequestHandler::RequestHandler(RIL *ril) : m_ril(ril){
    m_requestHandlers[RIL_REQUEST_BASEBAND_VERSION - FirstRequest] = &RequestHandler::handleBasebandVersion;
    m_requestHandlers[RIL_REQUEST_RADIO_POWER - FirstRequest] = &RequestHandler::handleRadioPower;
    m_requestHandlers[RIL_REQUEST_GET_IMEI - FirstRequest] = &RequestHandler::handleIMEI;
    m_requestHandlers[RIL_REQUEST_GET_IMSI - FirstRequest] = &RequestHandler::handleIMSI;
}

void RequestHandler::handle(Request *request) {
    if(m_ril->radioState() == RADIO_STATE_UNAVAILABLE ||
       (m_ril->radioState() == RADIO_STATE_OFF && request->code() != RIL_REQUEST_RADIO_POWER)) {

        request->complete(RIL_E_RADIO_NOT_AVAILABLE);

        return;
    }


    if(request->code() == RIL_REQUEST_GET_CURRENT_CALLS) {
        request->complete(RIL_E_RADIO_NOT_AVAILABLE);

        return;
    }

    if(!supports(request->code())) {
        Log::debug("Unsupported request %d:", request->code());

        const std::vector<char> &data = request->data();

        dump(&data[0], data.size());

        request->complete(RIL_E_REQUEST_NOT_SUPPORTED);
    } else {
        (this->*(m_requestHandlers[request->code() - FirstRequest]))(request);
    }

}

bool RequestHandler::supports(int request) {
    if(request < FirstRequest || request > LastRequest)
        return false;

    return m_requestHandlers[request - FirstRequest] != 0;
}

void RequestHandler::handle(SamsungIPC::Messages::MiscGetMobileEquipImsiReply *message) {
    Log::debug("Unsolicited MiscGetMobileEquipImsiReply");
}

void (RequestHandler::*RequestHandler::m_requestHandlers[LastRequest - FirstRequest + 1])(Request *request);