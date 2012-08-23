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
#include <limits.h>
#include <stdlib.h>

#include "RequestHandler.h"
#include "Request.h"
#include "RIL.h"

using namespace SamsungIPC;

void RequestHandler::handleSetMute(Request *request) {
    int mute = ((int *) request->data())[0];

    Messages::SndSetMicMute *message = new Messages::SndSetMicMute;
    message->setMute(mute != 0);
    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "SndSetMicMute", request);
}

void RequestHandler::handleGetMute(Request *request) {
    Message *reply = m_ril->execute(new Messages::SndGetMicMute);
    Messages::SndGetMicMuteReply *complete = message_cast<Messages::SndGetMicMuteReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to SndGetMicMute: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);
    } else {
        int mute = complete->mute() != 0;

        request->complete(RIL_E_SUCCESS, &mute, sizeof(mute));
    }

    delete reply;
}
