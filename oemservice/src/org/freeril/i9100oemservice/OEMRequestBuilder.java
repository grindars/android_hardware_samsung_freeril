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

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import android.util.Log;

public class OEMRequestBuilder {
    private static final String TAG = "I9100OemService";

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
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_ATTACH_SERVICE);
            dataStream.writeBoolean(attach);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public byte[] buildSetLoopbackTest(int loopback, int path) {
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_SET_LOOPBACK_TEST);
            dataStream.writeInt(loopback);
            dataStream.writeInt(path);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public byte[] buildSetDhaSolution(int mode, int select, String extra) {
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_SET_DHA_SOLUTION);
            dataStream.writeInt(mode);
            dataStream.writeInt(select);
            dataStream.writeUTF(extra);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public byte[] buildSetTwoMicControl(int device, int report) {
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_SET_TWO_MIC_CONTROL);
            dataStream.writeInt(device);
            dataStream.writeInt(report);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public byte[] buildGetMute() {
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_GET_MUTE);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public byte[] buildSetMute(int mute) {
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_SET_MUTE);
            dataStream.writeInt(mute);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public byte[] buildSetCallRecord(int record) {
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_SET_CALL_RECORD);
            dataStream.writeInt(record);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public byte[] buildSetCallClockSync(int sync) {
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_SET_CALL_CLOCK_SYNC);
            dataStream.writeInt(sync);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public byte[] buildSetVideoCallClockSync(int sync) {
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_SET_VIDEO_CALL_CLOCK_SYNC);
            dataStream.writeInt(sync);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public byte[] buildSetCallAudioPath(int path, int extraVolume) {
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_SET_CALL_AUDIO_PATH);
            dataStream.writeInt(path);
            dataStream.writeInt(extraVolume);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public byte[] buildSetCallVolume(int device, int volume) {
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_SET_CALL_VOLUME);
            dataStream.writeInt(device);
            dataStream.writeInt(volume);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public byte[] buildSamsungOemRequest(byte[] data) {
        try {
            ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
            DataOutputStream dataStream = new DataOutputStream(byteArrayStream);

            dataStream.writeInt(FREERIL_OEM_SIGNATURE_LOW);
            dataStream.writeInt(FREERIL_OEM_SIGNATURE_HIGH);

            dataStream.writeInt(OEM_REQUEST_SAMSUNG_OEM_REQUEST);
            dataStream.write(data, 0, data.length);

            return byteArrayStream.toByteArray();
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return null;
        }
    }

    public boolean parseGetMute(OemRequestReply reply, boolean[] muted) {
        try {
            ByteArrayInputStream byteArrayStream = reply.asInput();
            DataInputStream dataStream = new DataInputStream(byteArrayStream);

            muted[0] = dataStream.readBoolean();

            return true;
        } catch(IOException e) {
            Log.e("OEMRequestBuilder", "Unexpected exception: ", e);

            return false;
        }
    }
}

