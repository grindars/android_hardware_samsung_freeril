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

#include <Log.h>
#include <Messages.h>

#include "OemRequestHandler.h"
#include "RIL.h"
#include "RequestHandler.h"

using namespace SamsungIPC;

OemRequestHandler::OemRequestHandler(RIL *ril) : m_ril(ril) {

}

void OemRequestHandler::handleAttachService(bool attach) {
    Log::debug("handleAttachService(%d)", attach);

}

void OemRequestHandler::handleSetLoopbackTest(int path, int loopback) {
    Log::debug("handleSetLoopbackTest(%d, %d)", path, loopback);

    (void) loopback;
    Messages::SndSetLoopbackCtrl *message = new Messages::SndSetLoopbackCtrl;
    message->setCtrl(path);
    Message *reply = m_ril->execute(message);
    RequestHandler::completeGenCommand(reply, "SndSetLoopbackCtrl", 0);
}

void OemRequestHandler::handleSetDhaSolution(int mode, int select,
                          const std::string &extra) {

    Log::debug("handleSetDhaSolution(%d, %d, %s)", mode, select,
               extra.c_str());

    std::vector<unsigned char> parameter;
    parameter.resize(24);
    memcpy(&parameter[0], extra.c_str(), std::min(parameter.size(), extra.size() + 1));

    Messages::SndSetDhaCtrl *message = new Messages::SndSetDhaCtrl;
    message->setMode(mode);
    message->setSelect(select);
    message->setParameter(parameter);
    Message *reply = m_ril->execute(message);
    RequestHandler::completeGenCommand(reply, "SndSetDhaCtrl", 0);
}

void OemRequestHandler::handleSetTwoMicControl(int param1, int param2) {
    Log::debug("handleSetTwoMicControl(%d, %d)", param1, param2);

    Messages::SndSetTwoMicCtrl *message = new Messages::SndSetTwoMicCtrl;
    message->setParam1(param1);
    message->setParam2(param2);
    Message *reply = m_ril->execute(message);
    RequestHandler::completeGenCommand(reply, "SndSetTwoMicCtrl", 0);
}

bool OemRequestHandler::handleGetMute() {
    Log::debug("handleGetMute()");

    bool ret;

    Message *reply = m_ril->execute(new Messages::SndGetMicMute);
    Messages::SndGetMicMuteReply *complete = message_cast<Messages::SndGetMicMuteReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to SndGetMicMute: %s", reply->inspect().c_str());

        ret = false;
    } else {
        ret = complete->mute() != 0;
    }

    delete reply;

    return false;
}

void OemRequestHandler::handleSetMute(bool muted) {
    Log::debug("handleSetMute(%d)", muted);

    Messages::SndSetMicMute *message = new Messages::SndSetMicMute;
    message->setMute(muted != 0);
    Message *reply = m_ril->execute(message);
    RequestHandler::completeGenCommand(reply, "SndSetMicMute", 0);
}

void OemRequestHandler::handleSetCallRecord(int record) {
    Log::debug("handleSetCallRecord(%d)", record);

    Messages::SndSetVoiceRecordingCtrl *message = new Messages::SndSetVoiceRecordingCtrl;
    message->setRecord(record);
    Message *reply = m_ril->execute(message);
    RequestHandler::completeGenCommand(reply, "SndSetVoiceRecordingCtrl", 0);
}

void OemRequestHandler::handleSetCallClockSync(int sync) {
    Log::debug("handleSetCallClockSync(%d)", sync);

    Messages::SndExecClockCtrl *message = new Messages::SndExecClockCtrl;
    message->setClock(sync);
    Message *reply = m_ril->execute(message);
    RequestHandler::completeGenCommand(reply, "SndExecClockCtrl", 0);
}

void OemRequestHandler::handleSetVideoCallClockSync(int sync) {
    Log::debug("handleSetVideoCallClockSync(%d)", sync);

    Messages::SndSetVideoCallCtrl *message = new Messages::SndSetVideoCallCtrl;
    message->setClock(sync);
    Message *reply = m_ril->execute(message);
    RequestHandler::completeGenCommand(reply, "SndSetVideoCallCtrl", 0);
}

void OemRequestHandler::handleSetCallAudioPath(int path, int extraVolume) {
    Log::debug("handleSetCallAudioPath", path, extraVolume);

    static const unsigned char path_map[] = {
        1, 2, 6, 4, 5, 7, 8, 0
    };

    (void) extraVolume;

    if(path < 0 || path >= (int) sizeof(path_map)) {
        Log::warning("Invalid audio path %d", path);

        return;
    }

    Messages::SndSetAudioPath *message = new Messages::SndSetAudioPath;
    message->setPath(path_map[path]);
    Message *reply = m_ril->execute(message);
    RequestHandler::completeGenCommand(reply, "SndSetAudioPath", 0);
}

void OemRequestHandler::handleSetCallVolume(int device, int volume) {
    Log::debug("handleSetCallVolume(%d, %d)", device, volume);

    static const unsigned char device_map[] = {
        1, 17, 49, 65
    };

    if(device < 0 || device > (int) sizeof(device_map)) {
        Log::warning("Invalid device %d", device);

        return;
    }

    Messages::SndSetVolumeCtrl *message = new Messages::SndSetVolumeCtrl;
    message->setDevice(device_map[device]);
    message->setVolume(volume);
    Message *reply = m_ril->execute(message);
    RequestHandler::completeGenCommand(reply, "SndSetVolumeCtrl", 0);
}

bool OemRequestHandler::handleSamsungOemRequest(android::Parcel &request, android::Parcel &response) {
    Log::debug("handleSamsungOemRequest");

    return false;
}
