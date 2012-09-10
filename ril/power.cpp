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
    Messages::MiscSetDebugLevel *levelMessage = new Messages::MiscSetDebugLevel;
    levelMessage->setLevel(1);
    m_ril->submit(levelMessage);

    m_ril->setRadioState(RADIO_STATE_OFF);
}

void RequestHandler::handle(SamsungIPC::Messages::PwrPhonePoweredOff *message) {
    Log::info("Unsolicited phone power off");
}

void RequestHandler::handle(SamsungIPC::Messages::PwrPhoneReset *message) {
    Log::info("Unsolicited phone reset");
}

void RequestHandler::handle(SamsungIPC::Messages::PwrPhoneModeChanged *message) {
    switch(message->mode()) {
        case Messages::PwrPhoneModeChanged::Normal:
        {
            Messages::SecGetPinStatus *msg = new Messages::SecGetPinStatus;
            msg->subscribe(handlePinStatusRefreshComplete, this);
            m_ril->submit(msg);

            break;
        }

        case Messages::PwrPhoneModeChanged::LPM:
            m_ril->setRadioState(RADIO_STATE_OFF);

            break;
    }
}

static void handleRadioPowerSetModeComplete(Message *reply, void *arg) {
    Request *request = static_cast<Request *>(arg);

    RequestHandler::completeGenCommand(reply, "PwrPhoneSetMode", request);
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

        message->subscribe(handleRadioPowerSetModeComplete, request);
        m_ril->submit(message);

    } else {
        Log::info("Powering radio off");

        Message *reply = m_ril->execute(new Messages::PwrPhonePowerOff);
        if(completeGenCommand(reply, "PwrPhonePowerOff", request))
            m_ril->setRadioState(RADIO_STATE_UNAVAILABLE);
    }
}

