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

package org.freeril.i9100oemservice;

import android.os.Parcel;

public class OEMRequestBuilder {
    // 'FreeRIL!' in ASCII
    static final int FREERIL_OEM_SIGNATURE_LOW  = 0x65657246;
    static final int FREERIL_OEM_SIGNATURE_HIGH = 0x214c4952;

    static final int OEM_REQUEST_ATTACH_SERVICE = 1;

    static final int OEM_REQUEST_SET_LOOPBACK_TEST = 2;
    static final int OEM_REQUEST_SET_DHA_SOLUTION = 3;
    static final int OEM_REQUEST_SET_TWO_MIC_CONTROL = 4;
    static final int OEM_REQUEST_GET_MUTE = 5;
    static final int OEM_REQUEST_SET_MUTE = 6;
    static final int OEM_REQUEST_SET_CALL_RECORD = 7;
    static final int OEM_REQUEST_SET_CALL_CLOCK_SYNC = 8;
    static final int OEM_REQUEST_SET_VIDEO_CALL_CLOCK_SYNC = 9;
    static final int OEM_REQUEST_SET_CALL_AUDIO_PATH = 10;
    static final int OEM_REQUEST_SET_CALL_VOLUME = 11;

    static final int OEM_REQUEST_SAMSUNG_OEM_REQUEST = 12;

    public byte[] buildAttach(boolean attach) {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_ATTACH_SERVICE);
            parcel.writeInt(attach ? 1 : 0);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public byte[] buildSetLoopbackTest(int loopback, int path) {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_SET_LOOPBACK_TEST);
            parcel.writeInt(loopback);
            parcel.writeInt(path);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public byte[] buildSetDhaSolution(int mode, int select, String extra) {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_SET_DHA_SOLUTION);
            parcel.writeInt(mode);
            parcel.writeInt(select);
            parcel.writeString(extra);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public byte[] buildSetTwoMicControl(int device, int report) {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_SET_TWO_MIC_CONTROL);
            parcel.writeInt(device);
            parcel.writeInt(report);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public byte[] buildGetMute() {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_GET_MUTE);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public byte[] buildSetMute(int mute) {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_SET_MUTE);
            parcel.writeInt(mute);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public byte[] buildSetCallRecord(int record) {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_SET_CALL_RECORD);
            parcel.writeInt(record);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public byte[] buildSetCallClockSync(int sync) {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_SET_CALL_CLOCK_SYNC);
            parcel.writeInt(sync);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public byte[] buildSetVideoCallClockSync(int sync) {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_SET_VIDEO_CALL_CLOCK_SYNC);
            parcel.writeInt(sync);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public byte[] buildSetCallAudioPath(int path, int extraVolume) {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_SET_CALL_AUDIO_PATH);
            parcel.writeInt(path);
            parcel.writeInt(extraVolume);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public byte[] buildSetCallVolume(int device, int volume) {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_SET_CALL_VOLUME);
            parcel.writeInt(device);
            parcel.writeInt(volume);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public byte[] buildSamsungOemRequest(byte[] data) {
        Parcel parcel = Parcel.obtain();

        try {
            parcel.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            parcel.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            parcel.writeInt(OEM_REQUEST_SAMSUNG_OEM_REQUEST);
            parcel.writeByteArray(data);

            return parcel.marshall();
        } finally {
            parcel.recycle();
        }
    }

    public void parseGetMute(OemRequestReply reply, boolean[] muted) {
        Parcel parcel = Parcel.obtain();

        try {
            byte[] data = reply.toByteArray();
            parcel.unmarshall(data, 0, data.length);

            muted[0] = parcel.readInt() != 0;

        } finally {
            parcel.recycle();
        }
    }
}

