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

void RequestHandler::handle(Messages::SecSimCardType *message) {
    Log::debug("Unsolicited SecSimCardType. Card type: %u, ICC type: %u\n",
               message->cardType(), message->iccType());
}

void RequestHandler::handle(Messages::SecGetPinStatusReply *message) {
    switch(message->state()) {
        case Messages::SecGetPinStatusReply::Ready:
            m_ril->setRadioState(RADIO_STATE_SIM_READY);

            break;

        case Messages::SecGetPinStatusReply::SimUnavailable:
            m_ril->setRadioState(RADIO_STATE_SIM_NOT_READY);

            break;

        default:
            m_ril->setRadioState(RADIO_STATE_SIM_LOCKED_OR_ABSENT);

            break;
    }
}

static void translateSimStatus(Messages::SecGetPinStatusReply::State state, uint8_t xstate, Request *request) {
    RIL_CardStatus_v6 status;

    if(state == Messages::SecGetPinStatusReply::NoSimPresent) {
        status.card_state = RIL_CARDSTATE_ABSENT;
        status.num_applications = 0;
        status.gsm_umts_subscription_app_index = RIL_CARD_MAX_APPS;

    } else {
        status.card_state = RIL_CARDSTATE_PRESENT;
        status.num_applications = 1;
        status.gsm_umts_subscription_app_index = 0;
        status.applications[0].app_type = RIL_APPTYPE_SIM;
        status.applications[0].perso_substate = RIL_PERSOSUBSTATE_UNKNOWN;
        status.applications[0].aid_ptr = NULL;
        status.applications[0].app_label_ptr = NULL;
        status.applications[0].pin1_replaced = 0;

        switch(state) {
            case Messages::SecGetPinStatusReply::Ready:
                status.applications[0].app_state = RIL_APPSTATE_READY;
                status.applications[0].pin1 = RIL_PINSTATE_UNKNOWN;

                break;

            case Messages::SecGetPinStatusReply::PINLock:
                if(xstate == 0x01) {
                    status.applications[0].app_state = RIL_APPSTATE_PIN;
                    status.applications[0].pin1 = RIL_PINSTATE_ENABLED_NOT_VERIFIED;
                } else {
                    status.applications[0].app_state = RIL_APPSTATE_PUK;
                    status.applications[0].pin1 = RIL_PINSTATE_ENABLED_BLOCKED;
                }

                break;

            default:
                Log::error("Unsupported SIM state %02X, substate %02hhX", state, xstate);

            case Messages::SecGetPinStatusReply::SimUnavailable:
                status.applications[0].app_state = RIL_APPSTATE_DETECTED;
                status.applications[0].pin1 = RIL_PINSTATE_UNKNOWN;

                break;
        }

        status.applications[0].pin2 = RIL_PINSTATE_UNKNOWN;
    }

    status.universal_pin_state = RIL_PINSTATE_UNKNOWN;
    status.cdma_subscription_app_index = RIL_CARD_MAX_APPS;
    status.ims_subscription_app_index = RIL_CARD_MAX_APPS;

    request->complete(RIL_E_SUCCESS, &status, sizeof(RIL_CardStatus_v6));
}

void RequestHandler::handleSIMStatus(Request *request) {
    if(m_ril->radioState() == RADIO_STATE_UNAVAILABLE ||
       m_ril->radioState() == RADIO_STATE_OFF) {

        translateSimStatus(Messages::SecGetPinStatusReply::SimUnavailable, 0, request);

        return;
    }

    Message *reply = m_ril->execute(new Messages::SecGetPinStatus);
    Messages::SecGetPinStatusReply *complete = message_cast<Messages::SecGetPinStatusReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to SecGetPinStatus: %s", reply->inspect().c_str());

        translateSimStatus(Messages::SecGetPinStatusReply::SimUnavailable, 0, request);
    } else {
        translateSimStatus(complete->state(), complete->xstate(), request);
        handle(complete);
    }

    delete reply;
}

void RequestHandler::handlePinStatusRefreshComplete(SamsungIPC::Message *reply, void *arg) {
    Messages::SecGetPinStatusReply *complete = message_cast<Messages::SecGetPinStatusReply>(reply);
    RequestHandler *handler = static_cast<RequestHandler *>(arg);

    if(complete == NULL)
        Log::error("Got unexpected message in response to SecGetPinStatus: %s", reply->inspect().c_str());
    else
        handler->handle(complete);
}

void RequestHandler::handleEnterSIMPin(Request *request) {
    setPinStatus(request,
                 ((const char **) request->data())[0],
                 NULL,
                 Messages::SecSetPinStatus::EnterPin);
}

void RequestHandler::handleEnterSIMPuk(Request *request) {
    setPinStatus(request,
                 ((const char **) request->data())[0],
                 ((const char **) request->data())[1],
                 Messages::SecSetPinStatus::EnterPin);
}

void RequestHandler::handleEnterSIMPin2(Request *request) {
    setPinStatus(request,
                 ((const char **) request->data())[0],
                 NULL,
                 Messages::SecSetPinStatus::EnterPin2);
}

void RequestHandler::handleEnterSIMPuk2(Request *request) {
    setPinStatus(request,
                 ((const char **) request->data())[0],
                 ((const char **) request->data())[1],
                 Messages::SecSetPinStatus::EnterPin2);
}

void RequestHandler::setPinStatus(Request *request, const char *pin, const char *puk, int op) {
    size_t pin_len, puk_len;

    if(pin)
        pin_len = strlen(pin);
    else
        pin_len = 0;

    if(puk)
        puk_len = strlen(puk);
    else
        puk_len = 0;

    if(pin_len > 8 || puk_len > 8) {
        request->complete(RIL_E_GENERIC_FAILURE);

        return;
    }

    Log::debug("SetPinStatus: operation %d, pin: %s, puk: %s",
               op,
               pin ? pin : "(none)",
               puk ? puk : "(none)");

    std::vector<unsigned char> pin1(8), pin2(8);
    memcpy(&pin1[0], pin, pin_len);
    memcpy(&pin2[0], puk, puk_len);

    Messages::SecSetPinStatus *message = new Messages::SecSetPinStatus;
    message->setType(Messages::SecSetPinStatus::EnterPin);
    message->setPin1Length(pin_len);
    message->setPin2Length(puk_len);
    message->setPin1(pin1);
    message->setPin2(pin2);

    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "SecSetPinStatus", request);
}

