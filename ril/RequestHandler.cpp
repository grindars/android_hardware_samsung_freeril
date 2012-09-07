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
#include <Messages.h>

#include "RequestHandler.h"
#include "Request.h"
#include "RIL.h"
#include "OemRequestHandler.h"
#include "OemUnsolicitedBuilder.h"

using namespace SamsungIPC;

RequestHandler::RequestHandler(RIL *ril) : m_ril(ril), m_coarseRSSI(-1),
    m_lastCallFailCause(CALL_FAIL_ERROR_UNSPECIFIED) {

    m_requestHandlers[RIL_REQUEST_OEM_HOOK_RAW - FirstRequest] = &RequestHandler::handleOemHookRaw;

    m_requestHandlers[RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE - FirstRequest] = &RequestHandler::handleQueryNetworkSelectionMode;
    m_requestHandlers[RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC - FirstRequest] = &RequestHandler::handleSetNetworkSelectionAutomatic;
    m_requestHandlers[RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL - FirstRequest] = &RequestHandler::handleSetNetworkSelectionManual;
    m_requestHandlers[RIL_REQUEST_QUERY_AVAILABLE_NETWORKS - FirstRequest] = &RequestHandler::handleQueryAvailableNetworks;
    m_requestHandlers[RIL_REQUEST_DTMF_START - FirstRequest] = &RequestHandler::handleDTMFStart;
    m_requestHandlers[RIL_REQUEST_DTMF_STOP - FirstRequest] = &RequestHandler::handleDTMFStop;
    m_requestHandlers[RIL_REQUEST_BASEBAND_VERSION - FirstRequest] = &RequestHandler::handleBasebandVersion;
    m_requestHandlers[RIL_REQUEST_GET_IMEI - FirstRequest] = &RequestHandler::handleIMEI;
    m_requestHandlers[RIL_REQUEST_GET_IMEISV - FirstRequest] = &RequestHandler::handleIMEISV;
    m_requestHandlers[RIL_REQUEST_ANSWER - FirstRequest] = &RequestHandler::handleAnswer;

    m_requestHandlers[RIL_REQUEST_SCREEN_STATE - FirstRequest] = &RequestHandler::handleScreenState;

    m_requestHandlers[RIL_REQUEST_SET_BAND_MODE - FirstRequest] = &RequestHandler::handleSetBandMode;
    m_requestHandlers[RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE - FirstRequest] = &RequestHandler::handleQueryAvailableBandMode;

    m_requestHandlers[RIL_REQUEST_EXPLICIT_CALL_TRANSFER - FirstRequest] = &RequestHandler::handleExplicitCallTransfer;
    m_requestHandlers[RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE - FirstRequest] = &RequestHandler::handleSetPreferredNetworkType;
    m_requestHandlers[RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE - FirstRequest] = &RequestHandler::handleGetPreferredNetworkType;

    m_requestHandlers[RIL_REQUEST_SEPARATE_CONNECTION - FirstRequest] = &RequestHandler::handleSeparateConnection;
    m_requestHandlers[RIL_REQUEST_SET_MUTE - FirstRequest] = &RequestHandler::handleSetMute;
    m_requestHandlers[RIL_REQUEST_GET_MUTE - FirstRequest] = &RequestHandler::handleGetMute;

    m_oemHandler = new OemRequestHandler(ril);
    m_oemBuilder = new OemUnsolicitedBuilder(ril);
}

RequestHandler::~RequestHandler() {
    delete m_oemBuilder;
    delete m_oemHandler;
}

void RequestHandler::handle(Request *request) {
    if((m_ril->radioState() == RADIO_STATE_UNAVAILABLE ||
        (m_ril->radioState() == RADIO_STATE_OFF && request->code() != RIL_REQUEST_RADIO_POWER)) &&
        request->code() != RIL_REQUEST_GET_SIM_STATUS &&
        request->code() != RIL_REQUEST_OEM_HOOK_RAW) {

        request->complete(RIL_E_RADIO_NOT_AVAILABLE);

        return;
    }

    if(!supports(request->code())) {
        Log::debug("Unsupported request %d:", request->code());

        dump(request->data(), request->data_size());

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

bool RequestHandler::completeGenCommand(SamsungIPC::Message *reply, const char *name, Request *request) {
    bool ret = false;

    Messages::GenCommandComplete *complete = message_cast<Messages::GenCommandComplete>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to %s: %s", name, reply->inspect().c_str());

        if(request)
            request->complete(RIL_E_GENERIC_FAILURE);

    } else if(complete->status() == Messages::GenCommandComplete::Success) {

        if(request)
            request->complete(RIL_E_SUCCESS);

        ret = true;

    } else {
        Log::error("%s failed with status 0x%04X", name, complete->status());

        if(request) {
            switch(complete->status()) {
                case Messages::GenCommandComplete::IncorrectPin:
                    request->complete(RIL_E_PASSWORD_INCORRECT);

                    break;

                default:
                    request->complete(RIL_E_GENERIC_FAILURE);

                    break;
            }
        }
    }

    delete reply;

    return ret;
}

void (RequestHandler::*RequestHandler::m_requestHandlers[LastRequest - FirstRequest + 1])(Request *request) = {
    &RequestHandler::handleSIMStatus,
    &RequestHandler::handleEnterSIMPin,
    &RequestHandler::handleEnterSIMPuk,
    &RequestHandler::handleEnterSIMPin2,
    &RequestHandler::handleEnterSIMPuk2,
    &RequestHandler::handleChangeSIMPin,
    &RequestHandler::handleChangeSIMPin2,
    NULL, // RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION
    &RequestHandler::handleCurrentCalls,
    &RequestHandler::handleDial,
    &RequestHandler::handleIMSI,
    &RequestHandler::handleHangup,
    &RequestHandler::handleHangupWaitingOrBackground,
    &RequestHandler::handleHangupForegroundResumeBackground,
    &RequestHandler::handleSwitchWaitingOrHoldingAndActive,
    &RequestHandler::handleConference,
    &RequestHandler::handleUDUB,
    &RequestHandler::handleLastCallFailCause,
    &RequestHandler::handleSignalStrength,
    &RequestHandler::handleVoiceRegistrationState,
    &RequestHandler::handleDataRegistrationState,
    &RequestHandler::handleOperator,
    &RequestHandler::handleRadioPower,
    &RequestHandler::handleDTMF
};

