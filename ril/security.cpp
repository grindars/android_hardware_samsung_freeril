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

static int hex2val(char ch) {
    if(ch >= '0' && ch <= '9')
        return ch - '0';
    else if(ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    else if(ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    else
        return -1;
}

static char val2hex(int value) {
    if(value >= 0x0A)
        return value + 'A' - 10;
    else
        return value + '0';
}

static bool hex2bin(const std::string &hex, std::vector<unsigned char> &bin) {
    size_t len = hex.length();

    if(len & 1)
        return false;

    bin.resize(len >> 1);

    for(size_t i = 0; i < len; i += 2) {
        int upper = hex2val(hex[i]), lower = hex2val(hex[i + 1]);

        if(upper == -1 || lower == -1)
            return false;

        bin[i >> 1] = (upper << 4) | lower;
    }

    return true;
}

static void bin2hex(const std::vector<unsigned char> &bin, std::string &hex) {
    size_t len = bin.size();

    hex.resize(len * 2);

    for(size_t i = 0; i < len; i++) {
        unsigned char byte = bin[i];

        char high = val2hex((byte & 0xF0) >> 4);
        char low  = val2hex(byte & 0x0F);

        hex[i * 2] = high;
        hex[i * 2 + 1] = low;
    }
}

void RequestHandler::handle(Messages::SecSimCardType *message) {
    Log::info("Card type: %u, ICC type: %u",
               message->cardType(), message->iccType());
}

void RequestHandler::handle(Messages::SecGetPinStatusReply *message) {
    m_rilMutex.lock();

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

    m_rilMutex.unlock();
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
        unexpected("SecGetPinStatus", reply);

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
        unexpected("SecGetPinStatus", reply);
    else
        handler->handle(complete);
}

void RequestHandler::handleEnterSIMPin(Request *request) {
    setPinStatus(request,
                 ((const char **) request->data())[0],
                 NULL,
                 Messages::SecSetPinStatus::Pin);
}

void RequestHandler::handleEnterSIMPuk(Request *request) {
    setPinStatus(request,
                 ((const char **) request->data())[0],
                 ((const char **) request->data())[1],
                 Messages::SecSetPinStatus::Pin);
}

void RequestHandler::handleEnterSIMPin2(Request *request) {
    setPinStatus(request,
                 ((const char **) request->data())[0],
                 NULL,
                 Messages::SecSetPinStatus::Pin2);
}

void RequestHandler::handleEnterSIMPuk2(Request *request) {
    setPinStatus(request,
                 ((const char **) request->data())[0],
                 ((const char **) request->data())[1],
                 Messages::SecSetPinStatus::Pin2);
}

void RequestHandler::handleChangeSIMPin(Request *request) {
    changeLockPwd(request,
                  ((const char **) request->data())[0],
                  ((const char **) request->data())[1],
                  Messages::SecChangeLockPwd::Pin);
}

void RequestHandler::handleChangeSIMPin2(Request *request) {
    changeLockPwd(request,
                  ((const char **) request->data())[0],
                  ((const char **) request->data())[1],
                  Messages::SecChangeLockPwd::Pin2);
}

void RequestHandler::handleSIM_IO(Request *request) {
    const RIL_SIM_IO_v6 *io = (const RIL_SIM_IO_v6 *) request->data();

    if((io->command == 0xD6 || io->command == 0xDC) && io->fileid == 0x6F3B) {
        Log::debug("Write data to EF file");

    } else {
        Messages::SecRsimAccess *message = new Messages::SecRsimAccess;
        message->setCmd(io->command);
        message->setFileId(io->fileid);
        message->setP1(io->p1);
        message->setP2(io->p2);
        message->setP3(io->p3);

        std::vector<unsigned char> data;

        if(io->data && !hex2bin(io->data, data)) {
            request->complete(RIL_E_GENERIC_FAILURE);

            return;
        }

        message->setData(data);

        Message *reply = m_ril->execute(message);
        Messages::SecRsimAccessReply *complete = message_cast<Messages::SecRsimAccessReply>(reply);
        if(complete == NULL) {
            unexpected("SecRsimAccess", reply);

            request->complete(RIL_E_GENERIC_FAILURE);

            delete reply;

            return;
        }

        RIL_SIM_IO_Response response;
        response.sw1 = complete->sw1();
        response.sw2 = complete->sw2();

        data = complete->data();
        delete reply;

        if(data.size() == 0) {
            response.simResponse = NULL;
        } else {
            std::string hex;

            bin2hex(data, hex);

            response.simResponse = strdup(hex.c_str());
        }

        Log::debug("SIM_IO: 0x%02hhX, 0x%04hX, %s, 0x%02hhX, 0x%02hhX, 0x%02hhX, %s -> 0x%02hhX, 0x%02hhX, %s",
                   io->command, io->fileid, io->path, io->p1, io->p2, io->p3, io->data, response.sw1, response.sw2, response.simResponse);

        request->complete(RIL_E_SUCCESS, &response, sizeof(RIL_SIM_IO_Response));
        free(response.simResponse);
    }

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

    std::vector<unsigned char> pin1(8), pin2(8);
    memcpy(&pin1[0], pin, pin_len);
    memcpy(&pin2[0], puk, puk_len);

    Messages::SecSetPinStatus *message = new Messages::SecSetPinStatus;
    message->setType((Messages::SecSetPinStatus::Type) op);
    message->setPin1Length(pin_len);
    message->setPin2Length(puk_len);
    message->setPin1(pin1);
    message->setPin2(pin2);

    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "SecSetPinStatus", request);
}

void RequestHandler::changeLockPwd(Request *request, const char *currentPwd, const char *newPwd, int op) {
    size_t current_len, new_len;

    current_len = strlen(currentPwd);
    new_len = strlen(newPwd);

    if(current_len > 39 || new_len > 39) {
        request->complete(RIL_E_GENERIC_FAILURE);

        return;
    }

    std::vector<unsigned char> currentVector(39), newVector(39);
    memcpy(&currentVector[0], currentPwd, current_len);
    memcpy(&newVector[0], newPwd, new_len);

    Messages::SecChangeLockPwd *message = new Messages::SecChangeLockPwd;
    message->setType((Messages::SecChangeLockPwd::Type) op);
    message->setCurrentLength(current_len);
    message->setNewLength(new_len);
    message->setCurrentPwd(currentVector);
    message->setNewPwd(newVector);

    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "SecChangeLockPwd", request);
}
