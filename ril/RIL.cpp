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

#include <cutils/properties.h>
#include <sys/mman.h>

#include <Log.h>
#include <AndroidHAL.h>
#include <SamsungModem.h>

#include "RIL.h"
#include "RequestHandler.h"
#include "Request.h"
#include "Message.h"
#include "RILDatabase.h"

#if defined(PROFILING)
#include "timeval.h"

#error Packet inspection must be enabled for profiling
#endif

using namespace SamsungIPC;
using namespace HAL;

RIL::RIL(const struct RIL_Env *env) : m_env(env), m_radioState(RADIO_STATE_UNAVAILABLE) {

    m_database = new RILDatabase;
    m_handler = new RequestHandler(this);
    m_hal = new AndroidHAL;
    m_modem = new SamsungModem(m_hal, m_handler);
}

RIL::~RIL() {
    delete m_handler;
    delete m_modem;
    delete m_hal;
    delete m_database;
}

bool RIL::initialize(int argc, char **argv) {
    (void) argc;
    (void) argv;

    char shadow_value[PROPERTY_VALUE_MAX], value[PROPERTY_VALUE_MAX];
    property_get("ro.telephony.ril_class", shadow_value, "RIL");
    property_get("ril.override_class", value, shadow_value);

    if(strcmp(value, "RIL") != 0) {
        Log::error("Wrong Java-side RIL class selected: %s", value);

        return false;
    }

    if(!m_database->open()) {
        Log::error("Unable to open RIL database: %s", m_database->errorString().c_str());

        return false;
    }

    Log::info("Booting modem.");

    if(!m_modem->boot())
        return false;

    Log::info("Initializing IPC.\n");

    m_modem->initialize();

    return true;
}

void RIL::request(int request, void *data, size_t datalen, RIL_Token t) {
    m_handler->handle(new Request(request, data, datalen, t, this));
}

int RIL::supports(int requestCode) {
    return m_handler->supports(requestCode) ? 1 : 0;
}

void RIL::cancel(RIL_Token t) {
    (void) t;
}

const char *RIL::getVersion() {
    return "FreeRIL-I9100 (prerelease version)";
}

void RIL::complete(RIL_Token t, RIL_Errno e,
                   const void *response, size_t responselen) {

    m_env->OnRequestComplete(t, e, const_cast<void *>(response), responselen);
}

void RIL::unsolicited(int code, const void *data, size_t datalen) {
    m_env->OnUnsolicitedResponse(code, data, datalen);
}

void RIL::setRadioState(RIL_RadioState state) {
    if(state != m_radioState) {
        m_radioState = state;

        unsolicited(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED);
    }
}

void RIL::submit(SamsungIPC::Message *message) {
    m_modem->submit(message);
}


SamsungIPC::Message *RIL::execute(SamsungIPC::Message *message) {
    m_handler->unlockRIL();

#if defined(PROFILING)
    struct timeval req_start, req_end, elapsed;

    gettimeofday(&req_start, NULL);
    std::string inspected = message->inspect();
#endif

    ExecutionData data;
    data.ril = this;

    message->subscribe(onExecutionComplete, &data);
    m_modem->submit(message);

    m_executeSemaphore.take();

#if defined(PROFILING)
    gettimeofday(&req_end, NULL);
    timeval_subtract(&elapsed, &req_end, &req_start);

    unsigned long elapsed_milliseconds = elapsed.tv_sec * 1000 + elapsed.tv_usec / 1000;
    if(elapsed_milliseconds > 250) {
        Log::warning("Synchronous execution of following message taken %u milliseconds.");
        Log::warning("Consider rewriting corresponding handler asynchronously.");
        Log::warning("%s", inspected.c_str());
    }
#endif

    m_handler->lockRIL();

    return data.reply;
}

void RIL::onExecutionComplete(Message *reply, void *arg) {
    ExecutionData *data = static_cast<ExecutionData *>(arg);

    data->reply = reply;
    data->ril->m_executeSemaphore.give();
}

