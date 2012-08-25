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

#ifndef __FREERIL__ICROSS_CLIENT_SERVICE__H__
#define __FREERIL__ICROSS_CLIENT_SERVICE__H__

#include <binder/IInterface.h>

/*
 * See also ICrossClientService.aidl.
 */

namespace android {

    class ICrossClientService: public IInterface {
    public:
        DECLARE_META_INTERFACE(CrossClientService);

        virtual bool setLoopbackTest(int32_t loopback, int32_t path) = 0;
        virtual bool setDhaSolution(int32_t mode, int32_t select,
                                    const String16 &extra) = 0;
        virtual bool setTwoMicControl(int32_t device, int32_t report) = 0;
        virtual bool getMute(bool *muted) = 0;
        virtual bool setMute(bool muted) = 0;
        virtual bool setCallRecord(int32_t record) = 0;
        virtual bool setCallClockSync(int32_t sync) = 0;
        virtual bool setVideoCallClockSync(int32_t sync) = 0;
        virtual bool setCallAudioPath(int32_t path, int32_t extraVolume) = 0;
        virtual bool setCallVolume(int32_t device, int32_t volume) = 0;

        virtual bool samsungOemRequest(const void *data, size_t data_size,
                                       void **reply, size_t *reply_size) = 0;

        enum {
            TRANSACTION_setLoopbackTest = IBinder::FIRST_CALL_TRANSACTION,
            TRANSACTION_setDhaSolution,
            TRANSACTION_setTwoMicControl,
            TRANSACTION_getMute,
            TRANSACTION_setMute,
            TRANSACTION_setCallRecord,
            TRANSACTION_setCallClockSync,
            TRANSACTION_setVideoCallClockSync,
            TRANSACTION_setCallAudioPath,
            TRANSACTION_setCallVolume,
            TRANSACTION_samsungOemRequest
        };
    };
}

#endif

