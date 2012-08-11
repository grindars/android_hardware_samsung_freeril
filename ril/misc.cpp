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

#include <Utilities.h>
#include <Messages.h>

#include "RequestHandler.h"
#include "Request.h"
#include "RIL.h"

using namespace SamsungIPC;

void RequestHandler::handleBasebandVersion(Request *request) {
    Messages::MiscGetMobileEquipVersion *message = new Messages::MiscGetMobileEquipVersion;
    message->setReserved(0xFF);

    Message *reply = m_ril->execute(message);
    Messages::MiscGetMobileEquipVersionReply *complete = message_cast<Messages::MiscGetMobileEquipVersionReply>(reply);

    Log::debug("Reply to handleBasebandVersion got");

    if(complete == NULL) {
        Log::error("Got unexpected message in response to MiscGetMobileEquipVersion: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);
    } else {
        std::string sw_ver = std::string((const char *) &complete->softwareVersion()[0], complete->softwareVersion().size());
        std::string hw_ver = std::string((const char *) &complete->hardwareVersion()[0], complete->hardwareVersion().size());
        std::string rfcal_date = std::string((const char *) &complete->RFCalDate()[0], complete->RFCalDate().size());
        std::string product_code = std::string((const char *) &complete->productCode()[0], complete->productCode().size());
        std::string model = std::string((const char *) &complete->model()[0], complete->model().size());
        uint8_t prl_num = complete->PRLNum();

        Log::info(
            "Baseband software version: %s\n"
            "Baseband hardware version: %s\n"
            "Baseband RF calibrated on %s\n"
            "Baseband product code: %s\n"
            "Baseband model: %s\n"
            "Baseband PRL number: %hhu",
            sw_ver.c_str(), hw_ver.c_str(), rfcal_date.c_str(), product_code.c_str(),
                  model.c_str(), prl_num);

        request->complete(RIL_E_SUCCESS, sw_ver.c_str(), sizeof(char *));
    }

    delete reply;
}

void RequestHandler::handleIMEI(Request *request) {
    Messages::MiscGetMobileEquipSerialNumber *msg = new Messages::MiscGetMobileEquipSerialNumber;
    msg->setReserved(0x01);

    Message *reply = m_ril->execute(msg);
    Messages::MiscGetMobileEquipSerialNumberReply *complete = message_cast<Messages::MiscGetMobileEquipSerialNumberReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to MiscGetMobileEquipSerialNumber: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);
    } else {
        if(complete->stringLength() >= 32) {
            request->complete(RIL_E_GENERIC_FAILURE);

            return;
        }

        std::string serial = std::string((const char *) &complete->serial()[0], complete->serial().size());

        Log::info("IMEI: %s", serial.c_str());

        request->complete(RIL_E_SUCCESS, serial.c_str(), sizeof(char *));
    }

    delete reply;
}

void RequestHandler::handleIMEISV(Request *request) {
    request->complete(RIL_E_SUCCESS, "01", sizeof(char *));
}

void RequestHandler::handleIMSI(Request *request) {

    if(m_ril->radioState() != RADIO_STATE_SIM_READY) {
        request->complete(RIL_E_GENERIC_FAILURE);

        return;
    }

    Message *reply = m_ril->execute(new Messages::MiscGetMobileEquipImsi);
    Messages::MiscGetMobileEquipImsiReply *complete = message_cast<Messages::MiscGetMobileEquipImsiReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to MiscGetMobileEquipImsi: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);
    } else {
        std::string imsi = std::string((const char *) &complete->imsi()[0], complete->imsi().size());

        Log::info("IMSI: %s", imsi.c_str());

        request->complete(RIL_E_SUCCESS, imsi.c_str(), sizeof(char *));
    }

    delete reply;
}

void RequestHandler::handle(SamsungIPC::Messages::MiscGetMobileEquipImsiReply *message) {
    (void) message;
}
