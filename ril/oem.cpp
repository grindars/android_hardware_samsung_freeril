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
#include <freeril/oem.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
#include <utils/String16.h>
#include <arpa/inet.h>

#include "RequestHandler.h"
#include "Request.h"
#include "RIL.h"
#include "OemRequestHandler.h"
#include "OemUnsolicitedBuilder.h"

using namespace FreeRIL;
using namespace SamsungIPC;
using namespace android;

void RequestHandler::handleOemHookRaw(Request *request) {
    const OemHeader *header = (const OemHeader *) request->data();

    if(request->data_size() < sizeof(OemHeader) ||
       header->signature[0] != OEM_SIGNATURE_LOW ||
       header->signature[1] != OEM_SIGNATURE_HIGH) {

        Log::warning("Malformed OEM request");

        request->complete(RIL_E_GENERIC_FAILURE);

        return;
    }

    Parcel parcel;
    parcel.setData((const uint8_t *)(header + 1),
                   request->data_size() - sizeof(OemHeader));

    OemRequest requestCode = (OemRequest) header->request;

    if(requestCode != OEM_REQUEST_ATTACH_SERVICE &&
        (m_ril->radioState() == RADIO_STATE_UNAVAILABLE ||
         m_ril->radioState() == RADIO_STATE_OFF)) {

        request->complete(RIL_E_RADIO_NOT_AVAILABLE);

        return;
    }

    switch(requestCode) {
        case OEM_REQUEST_ATTACH_SERVICE:
        {
            int32_t attach = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleAttachService(attach != 0)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_SET_LOOPBACK_TEST:
        {
            int32_t path = parcel.readInt32();
            int32_t loopback = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleSetLoopbackTest(path, loopback)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_SET_DHA_SOLUTION:
        {
            int32_t mode = parcel.readInt32();
            int32_t select = parcel.readInt32();
            String8 extra = String8(parcel.readString16());

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleSetDhaSolution(mode, select,
                std::string(extra.string()))) {

                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_SET_TWO_MIC_CONTROL:
        {
            int32_t param1 = parcel.readInt32();
            int32_t param2 = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleSetTwoMicControl(param1, param2)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_GET_MUTE:
        {
            Parcel reply;
            bool muted;

            if(m_oemHandler->handleGetMute(&muted)) {
                reply.writeInt32(muted ? 1 : 0);

                request->complete(RIL_E_SUCCESS, reply.data(), reply.dataSize());
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }


            break;
        }

        case OEM_REQUEST_SET_MUTE:
        {
            int32_t mute = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleSetMute(mute != 0)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_SET_CALL_RECORD:
        {
            int32_t record = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleSetCallRecord(record)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_SET_CALL_CLOCK_SYNC:
        {
            int32_t sync = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleSetCallClockSync(sync)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_SET_VIDEO_CALL_CLOCK_SYNC:
        {
            int32_t sync = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleSetVideoCallClockSync(sync)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_SET_CALL_AUDIO_PATH:
        {
            int32_t path = parcel.readInt32();
            int32_t extraVolume = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleSetCallAudioPath(path, extraVolume)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_SET_CALL_VOLUME:
        {
            int32_t device = parcel.readInt32();
            int32_t volume = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleSetCallVolume(device, volume)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_SAMSUNG_OEM_REQUEST:
        {
            Parcel reply;

            if(m_oemHandler->handleSamsungOemRequest(parcel, reply)) {
                request->complete(RIL_E_SUCCESS, reply.data(), reply.dataSize());
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_ENTER_SERVICE_MODE:
        {
            int32_t modeType = parcel.readInt32();
            int32_t subType = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleEnterServiceMode(modeType, subType)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_EXIT_SERVICE_MODE:
        {
            int32_t modeType = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleExitServiceMode(modeType)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        case OEM_REQUEST_SEND_SERVICE_KEY_CODE:
        {
            int32_t keyCode = parcel.readInt32();

            if(parcel.errorCheck() != NO_ERROR) {
                request->complete(RIL_E_GENERIC_FAILURE);
            } else if(m_oemHandler->handleSendServiceKeyCode(keyCode)) {
                request->complete(RIL_E_SUCCESS);
            } else {
                request->complete(RIL_E_GENERIC_FAILURE);
            }

            break;
        }

        default:
            Log::warning("Unsupported OEM request %u", header->request);

            request->complete(RIL_E_REQUEST_NOT_SUPPORTED);

            break;
    }

    return;
}

void RequestHandler::handle(SamsungIPC::Messages::SvcDisplayScreen *message) {
    if(message->lineCount() == 0) {
        m_oemBuilder->notifyServiceCompleted();
    } else {
        if(message->lineCount() == 1) {
            const Messages::SvcDisplayScreen::LinesItem &line = message->lines()[0];

            if(line.unknown1() == 0 && line.unknown2() == 1)
                return;
        }

        std::vector<std::string> lines;
        lines.resize(message->lineCount());
        for(unsigned int i = 0, count = message->lineCount(); i < count; i++) {
            const Messages::SvcDisplayScreen::LinesItem &line = message->lines()[i];
            const std::vector<unsigned char> &data = line.line();

            lines[i].assign((char *) &data[0], strnlen((char *) &data[0], data.size()));
        }

        m_oemBuilder->notifyServiceDisplay(lines);
    }
}
