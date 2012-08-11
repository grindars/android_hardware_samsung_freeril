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

#ifndef __REQUEST_HANDLER__H__
#define __REQUEST_HANDLER__H__

#include <telephony/ril.h>

#include <IUnsolicitedReceiver.h>

namespace SamsungIPC {
    class Message;
}

class RIL;
class Request;

class RequestHandler: public SamsungIPC::IUnsolicitedReceiver {
public:
    RequestHandler(RIL *ril);

    virtual void handle(Request *request);
    virtual bool supports(int request);

    virtual void handle(SamsungIPC::Messages::PwrPhoneBootComplete *message);
    virtual void handle(SamsungIPC::Messages::PwrPhonePoweredOff *message);
    virtual void handle(SamsungIPC::Messages::PwrPhoneReset *message);
    virtual void handle(SamsungIPC::Messages::PwrPhoneModeChanged *message);
    virtual void handle(SamsungIPC::Messages::MiscGetMobileEquipImsiReply *message);
    virtual void handle(SamsungIPC::Messages::SecGetPinStatusReply *message);
    virtual void handle(SamsungIPC::Messages::SecSimCardType *message);

private:

    void handleRadioPower(Request *request);
    void handleBasebandVersion(Request *request);
    void handleIMEI(Request *request);
    void handleIMEISV(Request *request);
    void handleIMSI(Request *request);

    void handleSIMStatus(Request *request);
    void handleEnterSIMPin(Request *request);
    void handleEnterSIMPuk(Request *request);
    void handleEnterSIMPin2(Request *request);
    void handleEnterSIMPuk2(Request *request);
    void handleChangeSIMPin(Request *request);
    void handleChangeSIMPin2(Request *request);
    static void handlePinStatusRefreshComplete(SamsungIPC::Message *reply, void *arg);
    void setPinStatus(Request *request, const char *pin, const char *puk, int op);
    void changeLockPwd(Request *request, const char *currentPwd, const char *newPwd, int op);

    bool completeGenCommand(SamsungIPC::Message *reply, const char *name, Request *request);

    enum {
        FirstRequest = RIL_REQUEST_GET_SIM_STATUS,
        LastRequest = RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS
    };

    RIL *m_ril;

    static void (RequestHandler::*m_requestHandlers[LastRequest - FirstRequest + 1])(Request *request);
};

#endif
