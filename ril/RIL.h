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

#ifndef __RIL__H__
#define __RIL__H__

#include <telephony/ril.h>

#include "AndroidLogSink.h"
#include "ICompletionHandler.h"

namespace HAL {
    class AndroidHAL;
}

namespace SamsungIPC {
    class SamsungModem;
}

class RequestQueue;
class RequestHandler;

class RIL: public ICompletionHandler {
public:
    RIL(const struct RIL_Env *env);
    ~RIL();

    bool initialize(int argc, char **argv);

    void request(int request, void *data, size_t datalen, RIL_Token t);
    RIL_RadioState stateRequest();
    int supports(int requestCode);
    void cancel(RIL_Token t);
    const char *getVersion();

    virtual void completed(RIL_Token t, RIL_Errno e,
                           const void *response, size_t responselen);

private:
    AndroidLogSink m_sink;
    const struct RIL_Env *m_env;
    RIL_RadioState m_radioState;

    HAL::AndroidHAL *m_hal;
    SamsungIPC::SamsungModem *m_modem;
    RequestHandler *m_handler;
    RequestQueue *m_queue;
};

#endif
