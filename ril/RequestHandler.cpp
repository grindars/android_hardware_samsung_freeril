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

#include <cutils/properties.h>
#include <stdlib.h>
#include <unistd.h>

#include <Log.h>
#include <Utilities.h>
#include <Messages.h>

#include "RequestHandler.h"
#include "Request.h"
#include "RIL.h"

using namespace SamsungIPC;

RequestHandler::RequestHandler(RIL *ril) : m_ril(ril){
    m_requestHandlers[RIL_REQUEST_RADIO_POWER - FirstRequest] = &RequestHandler::handleRadioPower;
}

void RequestHandler::handle(Request *request) {
    if(m_ril->radioState() == RADIO_STATE_UNAVAILABLE ||
       (m_ril->radioState() == RADIO_STATE_OFF && request->code() != RIL_REQUEST_RADIO_POWER)) {

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

void RequestHandler::handle(SamsungIPC::Messages::PwrPhoneBootComplete *message) {
    m_ril->setRadioState(RADIO_STATE_OFF);
}

void RequestHandler::handle(SamsungIPC::Messages::PwrPhonePoweredOff *message) {
    Log::debug("Unsolicited phone power off");
}

void RequestHandler::handle(SamsungIPC::Messages::PwrPhoneReset *message) {
    Log::debug("Unsolicited phone reset");
}

void RequestHandler::handle(SamsungIPC::Messages::PwrPhoneModeChanged *message) {
    switch(message->mode()) {
        case Messages::PwrPhoneModeChanged::Normal:
            m_ril->setRadioState(RADIO_STATE_SIM_READY);

            break;

        case Messages::PwrPhoneModeChanged::LPM:
            m_ril->setRadioState(RADIO_STATE_OFF);

            break;
    }
}

void RequestHandler::handleRadioPower(Request *request) {
    const std::vector<char> &data = request->data();

    if(data.size() != sizeof(int)) {
        request->complete(RIL_E_GENERIC_FAILURE);

        return;
    }

    bool on = *(int *) &data[0] != 0;

    if((on && m_ril->radioState() != RADIO_STATE_OFF) ||
       (!on && m_ril->radioState() == RADIO_STATE_OFF)) {

        request->complete(RIL_E_SUCCESS);

        return;
    }

    char value[PROPERTY_VALUE_MAX];
    property_get("sys.deviceOffReq", value, "0");

    bool shutdown = atoi(value) != 0;

    if(on) {
        Messages::PwrPhoneSetMode *message = new Messages::PwrPhoneSetMode;
        message->setMode(Messages::PwrPhoneSetMode::Normal);
        message->setFlags(0x02);
        message->subscribe(modeSwitchComplete, request);

        m_ril->submit(message);

    } else if(!shutdown) {
        Messages::PwrPhoneSetMode *message = new Messages::PwrPhoneSetMode;
        message->setMode(Messages::PwrPhoneSetMode::LPM);
        message->setFlags(0x00);
        message->subscribe(modeSwitchComplete, request);

        m_ril->submit(message);

    } else {
        Log::info("Powering radio off");

        Messages::PwrPhonePowerOff *message = new Messages::PwrPhonePowerOff;
        message->subscribe(radioOffComplete, new RequestBinding(this, request));

        m_ril->submit(message);
    }
}

void RequestHandler::modeSwitchComplete(Message *reply, void *arg) {
    Request *request = static_cast<Request *>(arg);
    Messages::GenCommandComplete *complete = message_cast<Messages::GenCommandComplete>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to PwrPhoneSetMode: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);

    } else if(complete->status() == Messages::GenCommandComplete::SUCCESS) {

        request->complete(RIL_E_SUCCESS);

    } else {
        Log::error("PwrPhoneSetMode failed with status 0x%04X", complete->status());

        request->complete(RIL_E_GENERIC_FAILURE);
    }
}

void RequestHandler::radioOffComplete(SamsungIPC::Message *reply, void *arg) {
    RequestBinding *binding = static_cast<RequestBinding *>(arg);
    Request *request = binding->request;
    RequestHandler *handler = binding->handler;
    delete binding;

    Messages::GenCommandComplete *complete = message_cast<Messages::GenCommandComplete>(reply);
    if(complete == NULL) {
        Log::error("Got unexpected message in response to PwrPhonePowerOff: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);

    } else if(complete->status() == Messages::GenCommandComplete::SUCCESS) {

        request->complete(RIL_E_SUCCESS);

        handler->m_ril->setRadioState(RADIO_STATE_UNAVAILABLE);

    } else {
        Log::error("PwrPhonePowerOff failed with status 0x%04X", complete->status());

        request->complete(RIL_E_GENERIC_FAILURE);
    }
}


void (RequestHandler::*RequestHandler::m_requestHandlers[LastRequest - FirstRequest + 1])(Request *request);