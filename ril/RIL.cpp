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
#include <AndroidHAL.h>
#include <SamsungModem.h>

#include "RIL.h"
#include "RequestQueue.h"
#include "RequestHandler.h"

using namespace SamsungIPC;
using namespace HAL;

RIL::RIL(const struct RIL_Env *env) : m_env(env), m_radioState(RADIO_STATE_UNAVAILABLE),
    m_hal(new AndroidHAL), m_modem(new SamsungModem(m_hal)) {

    m_handler = new RequestHandler(this);
    m_queue = new RequestQueue(this, m_handler);
}

RIL::~RIL() {
    delete m_handler;
    delete m_queue;
    delete m_modem;
    delete m_hal;
}

bool RIL::initialize(int argc, char **argv) {
    (void) argc;
    (void) argv;

    Log::info("Booting modem.");

    if(!m_modem->boot())
        return false;

    Log::info("Initializing IPC.\n");

    m_modem->initialize();

    return true;
}

void RIL::request(int request, void *data, size_t datalen, RIL_Token t) {
    Log::debug("RIL::request(%d, %p, %u, %p)", request, data, datalen, t);

    m_queue->request(request, data, datalen, t);
}

RIL_RadioState RIL::stateRequest() {
    Log::debug("RIL::stateRequest() => %d", m_radioState);

    return m_radioState;
}

int RIL::supports(int requestCode) {
    int supports = 0;

    Log::debug("RIL::supports(%d) => %d\n", requestCode, supports);

    return supports;
}

void RIL::cancel(RIL_Token t) {
    Log::debug("RIL::cancel(%p)", t);

    m_queue->cancel(t);
}

const char *RIL::getVersion() {
    return "FreeRIL-I9100 (prerelease version)";
}

void RIL::completed(RIL_Token t, RIL_Errno e,
               const void *response, size_t responselen) {

    m_env->OnRequestComplete(t, e, const_cast<void *>(response), responselen);
}

