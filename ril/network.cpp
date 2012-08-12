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

#include <Messages.h>

#include "RequestHandler.h"
#include "RIL.h"
#include "Request.h"

using namespace SamsungIPC;

void RequestHandler::handleQueryNetworkSelectionMode(Request *request) {
    Message *reply = m_ril->execute(new Messages::NetGetPlmnSelection);
    Messages::NetGetPlmnSelectionReply *complete = message_cast<Messages::NetGetPlmnSelectionReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to NetGetPlmnSelectionReply: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);
    } else {
        int mode = complete->selection();

        if(mode >= 2 && mode <= 5) {
            int ril_mode;

            ril_mode = 0;

            request->complete(RIL_E_SUCCESS, &ril_mode, sizeof(int *));

        } else if(mode == 6) {
            int ril_mode;

            ril_mode = 1;

            request->complete(RIL_E_SUCCESS, &ril_mode, sizeof(int *));

        } else {
            request->complete(RIL_E_GENERIC_FAILURE);
        }
    }

    delete reply;
}


void RequestHandler::handleSetNetworkSelectionAutomatic(Request *request) {
    setPLMNSelection(request, NULL);
}

void RequestHandler::handleSetNetworkSelectionManual(Request *request) {
    setPLMNSelection(request, (const char *) request->data());
}

static char *getCleanMCCMNC(const void *data) {
    char *end = (char *) memrchr(data, '#', 6);
    size_t plmn_len;

    if(end == NULL)
        plmn_len = 6;
    else
        plmn_len = end - (char *) data;

    char *mccmnc = (char *) malloc(plmn_len + 1);
    memcpy(mccmnc, data, plmn_len);
    mccmnc[plmn_len] = 0;

    return mccmnc;
}

void RequestHandler::handleQueryAvailableNetworks(Request *request) {
    Message *reply = m_ril->execute(new Messages::NetGetPlmnList);
    Messages::NetGetPlmnListReply *complete = message_cast<Messages::NetGetPlmnListReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to NetGetPlmList: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);
    } else {
        struct PlmnRecord {
            uint8_t status;
            uint8_t plmn[6];
            uint8_t unknown[3];
        };

        const std::vector<unsigned char> &data = complete->data();

        if(complete->data().size() != sizeof(PlmnRecord) * complete->count()) {
            request->complete(RIL_E_GENERIC_FAILURE);

            delete reply;

            return;
        }

        const PlmnRecord *plmn = (const PlmnRecord *) &data[0];

        size_t response_size = sizeof(char **) * complete->count() * 5;
        char **response = (char **) malloc(response_size);

        for(size_t i = 0; i < complete->count(); i++) {
            char *mccmnc = getCleanMCCMNC(plmn[i].plmn);

            const char *status;
            switch(plmn[i].status) {
                case 2:
                    status = "available";

                    break;

                case 3:
                    status = "current";

                    break;

                case 4:
                    status = "forbidden";

                    break;

                default:
                    status = "unknown";

                    break;
            }

            // TODO: database lookup
            response[i * 5 + 0] = strdup(mccmnc);
            response[i * 5 + 1] = strdup(mccmnc);
            response[i * 5 + 2] = mccmnc;
            response[i * 5 + 3] = (char *) status;
            response[i * 5 + 4] = (char *) "";
        }

        request->complete(RIL_E_SUCCESS, response, response_size);

        for(size_t i = 0; i < complete->count(); i++) {
            free(response[i * 5 + 0]);
            free(response[i * 5 + 1]);
            free(response[i * 5 + 2]);
        }

        free(response);
    }

    delete reply;
}

void RequestHandler::handleOperator(Request *request) {
    Message *reply = m_ril->execute(new Messages::NetGetCurrentPlmn);
    Messages::NetGetCurrentPlmnReply *complete = message_cast<Messages::NetGetCurrentPlmnReply>(reply);
    if(complete == NULL) {
        Log::error("Got unexpected message in response to NetGetCurrentPlmn: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);
    } else {
        char *mccmnc = getCleanMCCMNC(&complete->plmn()[0]);

        // TODO: database lookup
        char *response[3] = { mccmnc, mccmnc, mccmnc };

        request->complete(RIL_E_SUCCESS, response, sizeof(response));

        free(mccmnc);
    }

    delete reply;
}

void RequestHandler::handle(SamsungIPC::Messages::NetGetCurrentPlmnReply *message) {
    (void) message;

    m_ril->unsolicited(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);
}

void RequestHandler::handleSetBandMode(Request *request) {
    Messages::NetSetBandSelection *message = new Messages::NetSetBandSelection;
    message->setUnknown1(0x02);

    const int *data = (const int *) request->data();
    switch(data[0]) {
        case 0:
        default:
            message->setBand(Messages::NetSetBandSelection::Automatic);

            break;

        case 1:
            message->setBand(Messages::NetSetBandSelection::EURO);

            break;

        case 2:
            message->setBand(Messages::NetSetBandSelection::US);

            break;

        case 3:
            message->setBand(Messages::NetSetBandSelection::JPN);

            break;

        case 4:
            message->setBand(Messages::NetSetBandSelection::AUS);

            break;

        case 5:
            message->setBand(Messages::NetSetBandSelection::AUS2);

            break;
    }

    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "NetSetBandSelection", request);
}

void RequestHandler::handleQueryAvailableBandMode(Request *request) {
    static const int data[7] = { 6, 0, 1, 2, 3, 4, 5 };

    request->complete(RIL_E_SUCCESS, data, sizeof(data));
}

void RequestHandler::handleSetPreferredNetworkType(Request *request) {
    int mode = ((const int *) request->data())[0];

    Messages::NetSetModeSelect *message = new Messages::NetSetModeSelect;

    switch(mode) {
        case PREF_NET_TYPE_LTE_GSM_WCDMA:
        case PREF_NET_TYPE_GSM_WCDMA_AUTO:
        case PREF_NET_TYPE_CDMA_EVDO_AUTO:
        case PREF_NET_TYPE_CDMA_ONLY:
        case PREF_NET_TYPE_EVDO_ONLY:
        case PREF_NET_TYPE_GSM_WCDMA:
            message->setMode(1);

            break;

        case PREF_NET_TYPE_GSM_ONLY:
            message->setMode(2);

            break;

        case PREF_NET_TYPE_WCDMA:
            message->setMode(3);

            break;

        case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
            message->setMode(4);

            break;

        case PREF_NET_TYPE_LTE_ONLY:
            message->setMode(6);

            break;

        default:
            message->setMode(7);

            break;
    }

    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "NetSetModeSelect", request);
}

void RequestHandler::handleGetPreferredNetworkType(Request *request) {
    Message *reply = m_ril->execute(new Messages::NetGetModeSelect);
    Messages::NetGetModeSelectReply *complete = message_cast<Messages::NetGetModeSelectReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to NetGetModeSelect: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);
    } else {
        int response;

        switch(complete->mode()) {
            case 1:
            case 3:
                response = PREF_NET_TYPE_GSM_ONLY;

                break;

            case 2:
                response = PREF_NET_TYPE_GSM_WCDMA;

                break;

            case 4:
                response = PREF_NET_TYPE_WCDMA;

                break;

            case 5:
                response = PREF_NET_TYPE_LTE_ONLY;

                break;

            case 6:
                response = PREF_NET_TYPE_CDMA_ONLY;

                break;

            default:
                response = -1;

                break;
        }

        request->complete(RIL_E_SUCCESS, &response, sizeof(response));
    }

    delete reply;
}

void RequestHandler::handleVoiceRegistrationState(Request *request) {
    getNetworkRegistration(request, Messages::NetGetNetworkRegistration::Voice);
}

void RequestHandler::handleDataRegistrationState(Request *request) {
    getNetworkRegistration(request, Messages::NetGetNetworkRegistration::Data);
}

void RequestHandler::handle(SamsungIPC::Messages::NetGetNetworkRegistrationReply *message) {
    switch(message->serviceDomain()) {
        case SamsungIPC::Messages::NetGetNetworkRegistrationReply::Voice:
            m_ril->unsolicited(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);

            break;

        case SamsungIPC::Messages::NetGetNetworkRegistrationReply::Data:

            break;

        default:
            Log::error("Registration of unknown service domain: %hhu", message->serviceDomain());
    }
}

static unsigned int toRilStatus(Messages::NetGetNetworkRegistrationReply::RegistrationStatus status) {
    switch(status) {
        case Messages::NetGetNetworkRegistrationReply::NotRegistered:
            return 0;

        case Messages::NetGetNetworkRegistrationReply::HomeNetwork:
            return 1;

        case Messages::NetGetNetworkRegistrationReply::SearchingEmergencyOnly:
            return 12;

        case Messages::NetGetNetworkRegistrationReply::RegistrationDeniedEmergencyOnly:
            return 13;

        case Messages::NetGetNetworkRegistrationReply::UnknownEmergencyOnly:
            return 14;

        case Messages::NetGetNetworkRegistrationReply::Roaming:
            return 5;

        case Messages::NetGetNetworkRegistrationReply::ANDROID_STATE_6:
            return 6;

        case Messages::NetGetNetworkRegistrationReply::GPRSNotAllowed:
            return 7;

        default:
            return -1;
    }
}

void RequestHandler::getNetworkRegistration(Request *request, int op) {
    Messages::NetGetNetworkRegistration *message = new Messages::NetGetNetworkRegistration;
    message->setUnknown1(0xFF);
    message->setServiceDomain((Messages::NetGetNetworkRegistration::ServiceDomain) op);

    Message *reply = m_ril->execute(message);
    Messages::NetGetNetworkRegistrationReply *complete = message_cast<Messages::NetGetNetworkRegistrationReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to NetGetNetworkRegistration: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);
    } else {
        char status[16], lac[16], cid[16];

        snprintf(status, sizeof(status), "%u", toRilStatus(complete->registrationStatus()));
        snprintf(lac, sizeof(lac), "%04hx", complete->lac());
        snprintf(cid, sizeof(cid), "%08x", complete->cid());

        char *response[] = {
            status,
            lac,
            cid
        };

        request->complete(RIL_E_SUCCESS, response, sizeof(response));
    }

    delete reply;
}

void RequestHandler::setPLMNSelection(Request *request, const char *plmn) {
    size_t plmn_length;

    if(plmn)
        plmn_length = strlen(plmn);
    else
        plmn_length = 0;

    if(plmn_length > 6) {
        request->complete(RIL_E_GENERIC_FAILURE);

        return;
    }

    std::vector<unsigned char> padded(6, '#');
    memcpy(&padded[0], plmn, plmn_length);

    Messages::NetSetPlmnSelection *message = new Messages::NetSetPlmnSelection;
    if(plmn)
        message->setSelection(Messages::NetSetPlmnSelection::Manual);
    else
        message->setSelection(Messages::NetSetPlmnSelection::Automatic);

    message->setNetwork(padded);
    message->setUnknown1(0xFF);

    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "NetSetPlmnSelection", request);
}

