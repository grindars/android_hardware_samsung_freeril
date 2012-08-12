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

#include "RequestHandler.h"
#include "Request.h"
#include "RIL.h"

using namespace SamsungIPC;

void RequestHandler::handleScreenState(Request *request) {
    request->complete(RIL_E_SUCCESS);
}

static void buildSignalStrength(int rssi, RIL_SignalStrength_v6 *strength) {
    strength->GW_SignalStrength.signalStrength = rssi;
    strength->GW_SignalStrength.bitErrorRate = 99;
    strength->LTE_SignalStrength.signalStrength = -1;
    strength->LTE_SignalStrength.rsrp = -1;
    strength->LTE_SignalStrength.rsrq = -1;
    strength->LTE_SignalStrength.rssnr = INT_MAX;
    strength->LTE_SignalStrength.cqi = -1;
}

static int translateBars(int bars) {
    bars = std::min(4, std::max<int>(0, bars));

    return bars * 31 / 5;
}

void RequestHandler::handleSignalStrength(Request *request) {
    Messages::DispGetIconInformation *message = new Messages::DispGetIconInformation;
    message->setIcon(Messages::DispGetIconInformation::SignalStrength);

    Message *reply = m_ril->execute(message);
    Messages::DispGetIconInformationReply *complete = message_cast<Messages::DispGetIconInformationReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to DispGetIconInformation: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);
    } else {
        RIL_SignalStrength_v6 strength;

        buildSignalStrength(translateBars(complete->value()), &strength);
        request->complete(RIL_E_SUCCESS, &strength, sizeof(strength));
    }

    delete reply;
}

void RequestHandler::handle(SamsungIPC::Messages::DispGetIconInformationReply *message) {
    if(message->icon() & 0x1) {
        RIL_SignalStrength_v6 strength;

        m_coarseRSSI = translateBars(message->value());

        buildSignalStrength(m_coarseRSSI, &strength);
        m_ril->unsolicited(RIL_UNSOL_SIGNAL_STRENGTH, &strength, sizeof(strength));
    }
}

void RequestHandler::handle(SamsungIPC::Messages::DispRssiInformation *message) {
    if(m_coarseRSSI != -1) {
        // TODO: extract fine RSSI level from DispRssiInformation

        RIL_SignalStrength_v6 strength;

        buildSignalStrength(m_coarseRSSI, &strength);

        m_ril->unsolicited(RIL_UNSOL_SIGNAL_STRENGTH, &strength, sizeof(strength));
    }

}

