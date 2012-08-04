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

RequestHandler::RequestHandler(RIL *ril) : m_ril(ril), m_radioIsOff(true) {
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

void RequestHandler::handle(SamsungIPC::Messages::PwrPhonePowerOnReply *message) {
    Log::debug("Modem started up. Notifying Android.");

    m_radioIsOff = false;

    m_ril->setRadioState(RADIO_STATE_OFF);
}

void RequestHandler::handle(SamsungIPC::Messages::PwrPhonePowerOffReply *message) {
    Log::debug("Unsolicited phone power off");
}

void RequestHandler::handle(SamsungIPC::Messages::PwrPhoneReset *message) {
    Log::debug("Unsolicited phone reset");
}

void RequestHandler::handleRadioPower(Request *request) {
    const std::vector<char> &data = request->data();

    if(data.size() != sizeof(int)) {
        request->complete(RIL_E_GENERIC_FAILURE);

        return;
    }

    bool on = *(int *) &data[0] != 0;

    Log::debug("Request for %d in state %d\n", on, m_ril->radioState());

    sleep(4);

    if((on && m_ril->radioState() != RADIO_STATE_OFF) ||
       (!on && m_ril->radioState() == RADIO_STATE_OFF)) {

        request->complete(RIL_E_SUCCESS);

        return;
    }

    on = false;

    char value[PROPERTY_VALUE_MAX];
    // TODO: sys.deviceOffReq
    property_get("ril.test_shutdown", value, "0");

    bool shutdown = atoi(value) != 0;

    if(on && m_radioIsOff) {
        Log::debug("Powering modem on");

        Messages::PwrPhonePowerOn *message = new Messages::PwrPhonePowerOn;
        message->subscribe(radioOnComplete, new RequestBinding(this, request));

        m_ril->submit(message);
    } else if(on) {
        Log::debug("Bringing radio back from the low power mode");

        Messages::PwrPhoneSetMode *message = new Messages::PwrPhoneSetMode;
        message->setMode(Messages::PwrPhoneSetMode::Normal);
        message->setFlags(0x02);
        message->subscribe(requestComplete, request);

        m_ril->submit(message);

    } else if(!shutdown) {
        Log::debug("Sending radio to the low power mode");

        Messages::PwrPhoneSetMode *message = new Messages::PwrPhoneSetMode;
        message->setMode(Messages::PwrPhoneSetMode::LPM);
        message->setFlags(0x00);
        message->subscribe(requestComplete, request);

        m_ril->submit(message);

    } else {
        Log::debug("Powering radio off");

        Messages::PwrPhonePowerOff *message = new Messages::PwrPhonePowerOff;
        message->subscribe(radioOffComplete, new RequestBinding(this, request));

        m_ril->submit(message);
    }
}

void RequestHandler::radioOnComplete(Message *reply, void *arg) {
    RequestBinding *binding = static_cast<RequestBinding *>(arg);

    Log::debug("radioOnComplete(%p, %p, %p)", reply, binding->handler, binding->request);

    binding->handler->m_radioIsOff = false;
    if(reply)
        binding->request->complete(RIL_E_SUCCESS);
    else
        binding->request->complete(RIL_E_GENERIC_FAILURE);

    delete binding;
}

void RequestHandler::radioOffComplete(Message *reply, void *arg) {
    RequestBinding *binding = static_cast<RequestBinding *>(arg);

    Log::debug("radioOffComplete(%p, %p, %p)", reply, binding->handler, binding->request);

    binding->handler->m_radioIsOff = true;
    if(reply)
        binding->request->complete(RIL_E_SUCCESS);
    else
        binding->request->complete(RIL_E_GENERIC_FAILURE);

    delete binding;
}

void RequestHandler::requestComplete(Message *reply, void *arg) {
    Log::debug("requestComplete(%p, %p)", reply, arg);

    if(reply)
        static_cast<Request *>(arg)->complete(RIL_E_SUCCESS);
    else
        static_cast<Request *>(arg)->complete(RIL_E_GENERIC_FAILURE);
}

void (RequestHandler::*RequestHandler::m_requestHandlers[LastRequest - FirstRequest + 1])(Request *request);