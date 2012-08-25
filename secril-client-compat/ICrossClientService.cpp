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

#include <freeril/ICrossClientService.h>
#include <binder/Parcel.h>

namespace android {

    class BpCrossClientService: public BpInterface<ICrossClientService> {
    public:
        BpCrossClientService(const sp<IBinder> &impl) : BpInterface<ICrossClientService>(impl) {

        }

        virtual bool setLoopbackTest(int32_t loopback, int32_t path) {
            Parcel data, reply;
            data.writeInterfaceToken(ICrossClientService::getInterfaceDescriptor());
            data.writeInt32(loopback);
            data.writeInt32(path);
            remote()->transact(TRANSACTION_setLoopbackTest, data, &reply);
            if(reply.readExceptionCode() != 0)
                return false;

            return reply.readInt32() != 0;
        }

        virtual bool setDhaSolution(int32_t mode, int32_t select,
                                    const String16 &extra) {
            Parcel data, reply;
            data.writeInterfaceToken(ICrossClientService::getInterfaceDescriptor());
            data.writeInt32(mode);
            data.writeInt32(select);
            data.writeString16(extra);
            remote()->transact(TRANSACTION_setDhaSolution, data, &reply);
            if(reply.readExceptionCode() != 0)
                return false;

            return reply.readInt32() != 0;
        }

        virtual bool setTwoMicControl(int32_t device, int32_t report) {
            Parcel data, reply;
            data.writeInterfaceToken(ICrossClientService::getInterfaceDescriptor());
            data.writeInt32(device);
            data.writeInt32(report);
            remote()->transact(TRANSACTION_setTwoMicControl, data, &reply);
            if(reply.readExceptionCode() != 0)
                return false;

            return reply.readInt32() != 0;
        }

        virtual bool getMute(bool *muted) {
            Parcel data, reply;
            data.writeInterfaceToken(ICrossClientService::getInterfaceDescriptor());
            data.writeInt32(1);
            data.writeInt32(0);
            remote()->transact(TRANSACTION_getMute, data, &reply);
            if(reply.readExceptionCode() != 0)
                return false;

            int status = reply.readInt32();

            if(reply.readInt32() != 1)
                return false;

            *muted = reply.readInt32() != 0;

            return status != 0;
        }

        virtual bool setMute(bool muted) {
            Parcel data, reply;
            data.writeInterfaceToken(ICrossClientService::getInterfaceDescriptor());
            data.writeInt32(muted ? 1 : 0);
            remote()->transact(TRANSACTION_setMute, data, &reply);
            if(reply.readExceptionCode() != 0)
                return false;

            return reply.readInt32() != 0;
        }

        virtual bool setCallRecord(int32_t record) {
            Parcel data, reply;
            data.writeInterfaceToken(ICrossClientService::getInterfaceDescriptor());
            data.writeInt32(record);
            remote()->transact(TRANSACTION_setCallRecord, data, &reply);
            if(reply.readExceptionCode() != 0)
                return false;

            return reply.readInt32() != 0;
        }

        virtual bool setCallClockSync(int32_t sync) {
            Parcel data, reply;
            data.writeInterfaceToken(ICrossClientService::getInterfaceDescriptor());
            data.writeInt32(sync);
            remote()->transact(TRANSACTION_setCallClockSync, data, &reply);
            if(reply.readExceptionCode() != 0)
                return false;

            return reply.readInt32() != 0;
        }

        virtual bool setVideoCallClockSync(int32_t sync) {
            Parcel data, reply;
            data.writeInterfaceToken(ICrossClientService::getInterfaceDescriptor());
            data.writeInt32(sync);
            remote()->transact(TRANSACTION_setVideoCallClockSync, data, &reply);
            if(reply.readExceptionCode() != 0)
                return false;

            return reply.readInt32() != 0;
        }

        virtual bool setCallAudioPath(int32_t path, int32_t extraVolume) {
            Parcel data, reply;
            data.writeInterfaceToken(ICrossClientService::getInterfaceDescriptor());
            data.writeInt32(path);
            data.writeInt32(extraVolume);
            remote()->transact(TRANSACTION_setCallAudioPath, data, &reply);
            if(reply.readExceptionCode() != 0)
                return false;

            return reply.readInt32() != 0;
        }

        virtual bool setCallVolume(int32_t device, int32_t volume) {
            Parcel data, reply;
            data.writeInterfaceToken(ICrossClientService::getInterfaceDescriptor());
            data.writeInt32(device);
            data.writeInt32(volume);
            remote()->transact(TRANSACTION_setCallVolume, data, &reply);
            if(reply.readExceptionCode() != 0)
                return false;

            return reply.readInt32() != 0;
        }

        virtual bool samsungOemRequest(const void *request_data, size_t request_size,
                                        void **reply_data, size_t *reply_size) {

            Parcel data, reply;
            data.writeInterfaceToken(ICrossClientService::getInterfaceDescriptor());
            data.writeInt32(request_size);
            data.write(request_data, request_size);
            remote()->transact(TRANSACTION_samsungOemRequest, data, &reply);
            if(reply.readExceptionCode() != 0)
                return false;

            int result = reply.readInt32();
            if(result == 0)
                return false;

            int dataPresent = reply.readInt32();

            if(dataPresent) {
                *reply_size = reply.readInt32();
                *reply_data = malloc(*reply_size);

                reply.read(*reply_data, *reply_size);

            } else {
                *reply_data = 0;
                *reply_size = 0;
            }

            return true;

        }
    };

    IMPLEMENT_META_INTERFACE(CrossClientService, "org.freeril.i9100oemservice.ICrossClientService");
}
