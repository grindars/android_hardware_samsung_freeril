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

#include <Utilities.h>
#include <Messages.h>

#include "RequestHandler.h"
#include "Request.h"
#include "RIL.h"

using namespace SamsungIPC;

void RequestHandler::handle(SamsungIPC::Messages::PwrPhoneBootComplete *message) {
    m_ril->setRadioState(RADIO_STATE_OFF);

    Messages::MiscSetDebugLevel *levelMessage = new Messages::MiscSetDebugLevel;
    levelMessage->setLevel(1);

    m_ril->submit(levelMessage);
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
            m_ril->setRadioState(RADIO_STATE_SIM_LOCKED_OR_ABSENT);

            break;

        case Messages::PwrPhoneModeChanged::LPM:
            m_ril->setRadioState(RADIO_STATE_OFF);

            break;
    }
}


void RequestHandler::handleRadioPower(Request *request) {
    if(request->data_size() != sizeof(int)) {
        request->complete(RIL_E_GENERIC_FAILURE);

        return;
    }

    bool on = *(int *)request->data() != 0;

    if((on && m_ril->radioState() != RADIO_STATE_OFF) ||
        (!on && m_ril->radioState() == RADIO_STATE_OFF)) {

        request->complete(RIL_E_SUCCESS);

        return;
    }

    char value[PROPERTY_VALUE_MAX];
    property_get("sys.deviceOffReq", value, "0");

    bool shutdown = atoi(value) != 0;

    if(on || !shutdown) {
        Messages::PwrPhoneSetMode *message = new Messages::PwrPhoneSetMode;
        if(on) {
            message->setMode(Messages::PwrPhoneSetMode::Normal);
            message->setFlags(0x02);
        } else {
            message->setMode(Messages::PwrPhoneSetMode::LPM);
            message->setFlags(0x00);
        }

        Message *reply = m_ril->execute(message);
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

        delete reply;

    } else {
        Log::info("Powering radio off");

        Message *reply = m_ril->execute(new Messages::PwrPhonePowerOff);
        Messages::GenCommandComplete *complete = message_cast<Messages::GenCommandComplete>(reply);

        if(complete == NULL) {
            Log::error("Got unexpected message in response to PwrPhonePowerOff: %s", reply->inspect().c_str());

            request->complete(RIL_E_GENERIC_FAILURE);
        } else if(complete->status() == Messages::GenCommandComplete::SUCCESS) {

            request->complete(RIL_E_SUCCESS);

            m_ril->setRadioState(RADIO_STATE_UNAVAILABLE);

        } else {
            Log::error("PwrPhonePowerOff failed with status 0x%04X", complete->status());

            request->complete(RIL_E_GENERIC_FAILURE);
        }

        delete reply;
    }
}

