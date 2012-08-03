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

#include "RIL.h"

extern "C" {
    const RIL_RadioFunctions *RIL_Init(const struct RIL_Env *env, int argc, char **argv) __attribute__((visibility("default")));
}

static RIL *ril;

static void requestThunk(int request, void *data, size_t datalen, RIL_Token t);
static RIL_RadioState stateRequestThunk();
static int supportsThunk(int requestCode);
static void cancelThunk(RIL_Token t);
static const char *getVersionThunk();

static const RIL_RadioFunctions functionTable = {
    RIL_VERSION,
    requestThunk,
    stateRequestThunk,
    supportsThunk,
    cancelThunk,
    getVersionThunk
};

const RIL_RadioFunctions *RIL_Init(const struct RIL_Env *env, int argc, char **argv) {
    ril = new RIL(env);

    SamsungIPC::Log::info("%s.", ril->getVersion());
    SamsungIPC::Log::info("This program is distributed in the hope that it will be useful,");
    SamsungIPC::Log::info("but WITHOUT ANY WARRANTY; without even the implied warranty of");
    SamsungIPC::Log::info("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the");
    SamsungIPC::Log::info("GNU General Public License for more details.");

    if(!ril->initialize(argc, argv)) {
        SamsungIPC::Log::error("RIL initialization failed.");

        return NULL;
    }

    SamsungIPC::Log::info("Initial RIL initialization completed.");

    return &functionTable;
}

static void requestThunk(int request, void *data, size_t datalen, RIL_Token t) {
    ril->request(request, data, datalen, t);
}

static RIL_RadioState stateRequestThunk() {
    return ril->radioState();
}

static int supportsThunk(int requestCode) {
    return ril->supports(requestCode);
}

static void cancelThunk(RIL_Token t) {
    ril->cancel(t);
}

static const char *getVersionThunk() {
    return ril->getVersion();
}
