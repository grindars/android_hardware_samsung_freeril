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

import android.util.Log;
import android.os.Process;

public class CrossClientService extends ICrossClientService.Stub {
    private RilInterface mRIL;

    public static final String LOG_TAG = "I9100OemService";

    public static final int CALL_AUDIO = 1;
    public static final int CALL_GPS = 2;

    public static final int DO_EVERYTHING = CALL_AUDIO | CALL_GPS;

    public CrossClientService(RilInterface ril) {
        super();

        mRIL = ril;
    }

    @Override
    public boolean setLoopbackTest(int loopback, int path) {
        if(!hasPermission(CALL_AUDIO))
            return false;

        return mRIL.setLoopbackTest(loopback, path);
    }

    @Override
    public boolean setDhaSolution(int mode, int select, String extra) {
        if(!hasPermission(CALL_AUDIO))
            return false;

        return mRIL.setDhaSolution(mode, select, extra);
    }

    @Override
    public boolean setTwoMicControl(int device, int report) {
        if(!hasPermission(CALL_AUDIO))
            return false;

        return mRIL.setTwoMicControl(device, report);
    }

    @Override
    public boolean getMute(boolean[] muted) {
        if(!hasPermission(CALL_AUDIO))
            return false;

        return mRIL.getMute(muted);
    }

    @Override
    public boolean setMute(int mute) {
        if(!hasPermission(CALL_AUDIO))
            return false;

        return mRIL.setMute(mute);
    }

    @Override
    public boolean setCallRecord(int record) {
        if(!hasPermission(CALL_AUDIO))
            return false;

        return mRIL.setCallRecord(record);
    }

    @Override
    public boolean setCallClockSync(int sync) {
        if(!hasPermission(CALL_AUDIO))
            return false;

        return mRIL.setCallClockSync(sync);
    }

    @Override
    public boolean setVideoCallClockSync(int sync) {
        if(!hasPermission(CALL_AUDIO))
            return false;

        return mRIL.setVideoCallClockSync(sync);
    }

    @Override
    public boolean setCallAudioPath(int path, int extraVolume) {
        if(!hasPermission(CALL_AUDIO))
            return false;

        return mRIL.setCallAudioPath(path, extraVolume);
    }

    @Override
    public boolean setCallVolume(int device, int volume) {
        if(!hasPermission(CALL_AUDIO))
            return false;

        return mRIL.setCallVolume(device, volume);
    }

    @Override
    public boolean samsungOemRequest(byte[] data, OemRequestReply reply) {
        if(!hasPermission(CALL_GPS))
            return false;

        return mRIL.samsungOemRequest(data, reply);
    }

    protected boolean hasPermission(int permissions) {
        boolean ret = (callerPermissions() & permissions) == permissions;

        if(!ret)
            Log.w(LOG_TAG, "Access denied for " + getCallingUid() + " upon request of " + permissions);

        return ret;
    }

    private int callerPermissions() {
        int uid = getCallingUid();

        if(uid == 0 || uid == Process.PHONE_UID) {
            return DO_EVERYTHING;

        } else if(uid == Process.SYSTEM_UID) {
            return CALL_GPS;

        } else if(uid == Process.getUidForName("media")) {
            return CALL_AUDIO;

        } else {
            return 0;
        }
    }
}
