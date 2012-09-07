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

bool OemRequestHandler::handleAttachService(bool attach) {
    (void) attach;

    return true;

}

bool OemRequestHandler::handleSetLoopbackTest(int path, int loopback) {
    (void) loopback;
    Messages::SndSetLoopbackCtrl *message = new Messages::SndSetLoopbackCtrl;
    message->setCtrl(path);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SndSetLoopbackCtrl", 0);
}

bool OemRequestHandler::handleSetDhaSolution(int mode, int select,
                          const std::string &extra) {
    std::vector<unsigned char> parameter;
    parameter.resize(24);
    memcpy(&parameter[0], extra.c_str(), std::min(parameter.size(), extra.size() + 1));

    Messages::SndSetDhaCtrl *message = new Messages::SndSetDhaCtrl;
    message->setMode(mode);
    message->setSelect(select);
    message->setParameter(parameter);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SndSetDhaCtrl", 0);
}

bool OemRequestHandler::handleSetTwoMicControl(int param1, int param2) {
    Messages::SndSetTwoMicCtrl *message = new Messages::SndSetTwoMicCtrl;
    message->setParam1(param1);
    message->setParam2(param2);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SndSetTwoMicCtrl", 0);
}

bool OemRequestHandler::handleGetMute(bool *muted) {
    Message *reply = m_ril->execute(new Messages::SndGetMicMute);
    Messages::SndGetMicMuteReply *complete = message_cast<Messages::SndGetMicMuteReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to SndGetMicMute: %s", reply->inspect().c_str());

        return false;
    } else {
        *muted = complete->mute() != 0;
    }

    return true;
}

bool OemRequestHandler::handleSetMute(bool muted) {
    Messages::SndSetMicMute *message = new Messages::SndSetMicMute;
    message->setMute(muted != 0);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SndSetMicMute", 0);
}

bool OemRequestHandler::handleSetCallRecord(int record) {
    Messages::SndSetVoiceRecordingCtrl *message = new Messages::SndSetVoiceRecordingCtrl;
    message->setRecord(record);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SndSetVoiceRecordingCtrl", 0);
}

bool OemRequestHandler::handleSetCallClockSync(int sync) {
    Messages::SndExecClockCtrl *message = new Messages::SndExecClockCtrl;
    message->setClock(sync);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SndExecClockCtrl", 0);
}

bool OemRequestHandler::handleSetVideoCallClockSync(int sync) {
    Messages::SndSetVideoCallCtrl *message = new Messages::SndSetVideoCallCtrl;
    message->setClock(sync);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SndSetVideoCallCtrl", 0);
}

bool OemRequestHandler::handleSetCallAudioPath(int path, int extraVolume) {
    static const unsigned char path_map[] = {
        1, 2, 6, 4, 5, 7, 8, 0
    };

    (void) extraVolume;

    if(path < 0 || path >= (int) sizeof(path_map)) {
        Log::warning("Invalid audio path %d", path);

        return false;
    }

    Messages::SndSetAudioPath *message = new Messages::SndSetAudioPath;
    message->setPath(path_map[path]);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SndSetAudioPath", 0);
}

bool OemRequestHandler::handleSetCallVolume(int device, int volume) {
    static const unsigned char device_map[] = {
        1, 17, 49, 65
    };

    if(device < 0 || device > (int) sizeof(device_map)) {
        Log::warning("Invalid device %d", device);

        return false;
    }

    Messages::SndSetVolumeCtrl *message = new Messages::SndSetVolumeCtrl;
    message->setDevice(device_map[device]);
    message->setVolume(volume);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SndSetVolumeCtrl", 0);
}

bool OemRequestHandler::handleSamsungOemRequest(android::Parcel &request, android::Parcel &response) {
    Log::debug("handleSamsungOemRequest");

    return false;
}

bool OemRequestHandler::handleEnterServiceMode(int modeType, int subType) {
    Messages::SvcEnterServiceMode *message = new Messages::SvcEnterServiceMode;
    message->setModeType(modeType);
    if(modeType == 1)
        message->setSubType(subType + 0x1000);
    else
        message->setSubType(0);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SvcEnterServiceMode", 0);
}

bool OemRequestHandler::handleExitServiceMode(int modeType) {
    Messages::SvcExitServiceMode *message = new Messages::SvcExitServiceMode;
    message->setModeType(modeType);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SvcExitServiceMode", 0);
}

bool OemRequestHandler::handleSendServiceKeyCode(int keyCode) {
    Messages::SvcProcessKeyCode *message = new Messages::SvcProcessKeyCode;
    message->setKeyCode(keyCode);
    Message *reply = m_ril->execute(message);
    return RequestHandler::completeGenCommand(reply, "SvcProcessKeyCode", 0);
}
