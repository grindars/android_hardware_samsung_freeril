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
