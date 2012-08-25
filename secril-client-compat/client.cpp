/*
 * libsecril-client compatibility library for FreeRIL-I9100.
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

#define LOG_TAG "RILCL"

#include <map>
#include <binder/IServiceManager.h>
#include <secril-client-compat.h>
#include <freeril/ICrossClientService.h>
#include <telephony/ril.h>

using namespace android;

struct ril_client {
    sp<ICrossClientService> interface;
    std::map<int, ril_client_complete_callback_t> callbacks;
};

ril_client_t *OpenClient_RILD(void) {
    ril_client_t *client = new ril_client_t;
    client->interface = NULL;

    return client;
}

ril_client_status_t CloseClient_RILD(ril_client_t *client) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface != NULL)
        Disconnect_RILD(client);

    return RIL_CLIENT_SUCCESS;
}

ril_client_status_t Connect_RILD(ril_client_t *client) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface != NULL)
        Disconnect_RILD(client);

    status_t status = getService<ICrossClientService>(
        String16("org.freeril.i9100oemservice.CrossClientService"),
        &client->interface);

    if(status == NO_ERROR) {
        LOGD("Connected to the service");

        return RIL_CLIENT_SUCCESS;
    } else {
        LOGD("Service is not available");

        return RIL_CLIENT_REFUSED;
    }
}

ril_client_status_t Disconnect_RILD(ril_client_t *client) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_UNKNOWN;

    client->interface.clear();

    LOGD("Disconnected from the service");

    return RIL_CLIENT_SUCCESS;
}

ril_client_status_t isConnected_RILD(ril_client_t *client) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return (ril_client_status_t) 0;

    status_t status = client->interface->asBinder()->pingBinder();

    if(status != NO_ERROR) {
        LOGD("Connection to the service lost");

        client->interface.clear();

        return (ril_client_status_t) 0;
    }

    return (ril_client_status_t) 1;
}

ril_client_status_t RegisterUnsolicitedHandler(ril_client_t *client, int code,
                                               ril_client_unsolicited_callback_t callback) {

    (void) client;
    (void) code;
    (void) callback;

    return RIL_CLIENT_SUCCESS;
}

ril_client_status_t RegisterRequestCompleteHandler(ril_client_t *client, int code,
                                                   ril_client_complete_callback_t callback) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(callback == NULL) {
        std::map<int, ril_client_complete_callback_t>::iterator it =
            client->callbacks.find(code);

        if(it != client->callbacks.end())
            client->callbacks.erase(it);

    } else {
        client->callbacks.insert(std::pair<int, ril_client_complete_callback_t>(code, callback));
    }

    return RIL_CLIENT_SUCCESS;
}

ril_client_status_t RegisterErrorCallback(ril_client_t *client, ril_client_error_callback_t callback,
                                          void *arg) {

    (void) client;
    (void) callback;
    (void) arg;

    return RIL_CLIENT_SUCCESS;
}

ril_client_status_t InvokeOemRequestHookRaw(ril_client_t *client, const void *data, size_t data_size) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_REFUSED;

    std::map<int, ril_client_complete_callback_t>::iterator it =
    client->callbacks.find(RIL_REQUEST_OEM_HOOK_RAW);

    if(it == client->callbacks.end()) {
        LOGW("Completion handler isn't installed for %d", RIL_REQUEST_OEM_HOOK_RAW);

        return RIL_CLIENT_UNKNOWN;
    }

    void *reply;
    size_t reply_size;

    bool ret = client->interface->samsungOemRequest(data, data_size, &reply, &reply_size);
    if(!ret) {
        LOGW("samsungOemRequest failed");

        return RIL_CLIENT_IO_ERROR;
    } else {
        (*it).second(client, reply, reply_size);
        free(reply);

        return RIL_CLIENT_SUCCESS;
    }
}

ril_client_status_t SetLoopbackTest(ril_client_t *client, int loopback, int path) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_REFUSED;

    bool ret = client->interface->setLoopbackTest(loopback, path);
    if(!ret) {
        LOGW("SetLoopbackTest(%d) failed", loopback);

        return RIL_CLIENT_IO_ERROR;
    } else {
        return RIL_CLIENT_SUCCESS;
    }
}

ril_client_status_t SetDhaSolution(ril_client_t *client, int mode, int select, const char *extra) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_REFUSED;

    bool ret = client->interface->setDhaSolution(mode, select, extra ? String16(extra) : String16(""));
    if(!ret) {
        LOGW("SetDhaSolution(%d, %d, %s) failed", mode, select, extra ? extra : "(null)");

        return RIL_CLIENT_IO_ERROR;
    } else {
        return RIL_CLIENT_SUCCESS;
    }
}

ril_client_status_t SetTwoMicControl(ril_client_t *client, int param1, int param2) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_REFUSED;

    bool ret = client->interface->setTwoMicControl(param1, param2);
    if(!ret) {
        LOGW("SetTwoMicControl(%d, %d) failed", param1, param2);

        return RIL_CLIENT_IO_ERROR;
    } else {
        return RIL_CLIENT_SUCCESS;
    }
}

ril_client_status_t GetMute(ril_client_t *client, ril_client_complete_callback_t callback) {

    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_REFUSED;

    bool muted, ret = client->interface->getMute(&muted);
    if(!ret) {
        LOGW("GetMute() failed");

        return RIL_CLIENT_IO_ERROR;
    } else {
        unsigned char byte = ret ? 1 : 0;

        callback(client, &byte, sizeof(byte));

        return RIL_CLIENT_SUCCESS;
    }

}

ril_client_status_t SetMute(ril_client_t *client, int mute) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_REFUSED;

    bool ret = client->interface->setMute(mute != 0);
    if(!ret) {
        LOGW("SetMute(%d) failed", mute);

        return RIL_CLIENT_IO_ERROR;
    } else {
        return RIL_CLIENT_SUCCESS;
    }
}


ril_client_status_t SetCallRecord(ril_client_t *client, int record) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_REFUSED;

    bool ret = client->interface->setCallRecord(record);
    if(!ret) {
        LOGW("SetCallRecord(%d) failed", record);

        return RIL_CLIENT_IO_ERROR;
    } else {
        return RIL_CLIENT_SUCCESS;
    }
}

ril_client_status_t SetCallClockSync(ril_client_t *client, int sync) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_REFUSED;

    bool ret = client->interface->setCallClockSync(sync);
    if(!ret) {
        LOGW("SetCallClockSync(%d) failed", sync);

        return RIL_CLIENT_IO_ERROR;
    } else {
        return RIL_CLIENT_SUCCESS;
    }
}

ril_client_status_t SetVideoCallClockSync(ril_client_t *client, int sync) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_REFUSED;

    bool ret = client->interface->setVideoCallClockSync(sync);
    if(!ret) {
        LOGW("SetVideoCallClockSync(%d) failed", sync);

        return RIL_CLIENT_IO_ERROR;
    } else {
        return RIL_CLIENT_SUCCESS;
    }
}

ril_client_status_t SetCallAudioPath(ril_client_t *client, int path, int extraVolume) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_REFUSED;

    bool ret = client->interface->setCallAudioPath(path, extraVolume);
    if(!ret) {
        LOGW("SetCallAudioPath(%d, %d) failed", path, extraVolume);

        return RIL_CLIENT_IO_ERROR;
    } else {
        return RIL_CLIENT_SUCCESS;
    }
}

ril_client_status_t SetCallVolume(ril_client_t *client, int device, int volume) {
    if(client == NULL)
        return RIL_CLIENT_INVALID;

    if(client->interface == NULL)
        return RIL_CLIENT_REFUSED;

    bool ret = client->interface->setCallVolume(device, volume);
    if(!ret) {
        LOGW("SetCallVolume(%d, %d) failed", device, volume);

        return RIL_CLIENT_IO_ERROR;
    } else {
        return RIL_CLIENT_SUCCESS;
    }
}
