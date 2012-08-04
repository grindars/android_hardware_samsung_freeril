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

#include "IRequestHandler.h"
#include <IUnsolicitedReceiver.h>

namespace SamsungIPC {
    class Message;
}

class RIL;

class RequestHandler: public IRequestHandler, public SamsungIPC::IUnsolicitedReceiver {
public:
    RequestHandler(RIL *ril);

    virtual void handle(Request *request);
    virtual bool supports(int request);

    virtual void handle(SamsungIPC::Messages::PwrPhonePowerOnReply *message);
    virtual void handle(SamsungIPC::Messages::PwrPhonePowerOffReply *message);
    virtual void handle(SamsungIPC::Messages::PwrPhoneReset *message);

private:
    struct RequestBinding {
        RequestBinding(RequestHandler *_handler, Request *_request) : handler(_handler), request(_request) {

        }

        RequestHandler *handler;
        Request *request;
    };

    static void radioOnComplete(SamsungIPC::Message *reply, void *arg);
    static void radioOffComplete(SamsungIPC::Message *reply, void *arg);
    void handleRadioPower(Request *request);

    static void requestComplete(SamsungIPC::Message *reply, void *arg);

    enum {
        FirstRequest = RIL_REQUEST_GET_SIM_STATUS,
        LastRequest = RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS
    };

    RIL *m_ril;

    static void (RequestHandler::*m_requestHandlers[LastRequest - FirstRequest + 1])(Request *request);
    bool m_radioIsOff;
};

#endif
