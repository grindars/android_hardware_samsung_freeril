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

import android.os.Handler;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.PhoneBase;
import com.android.internal.telephony.PhoneProxy;
import com.android.internal.telephony.CommandsInterface;
import android.os.Message;
import android.util.Log;
import android.os.AsyncResult;
import java.util.concurrent.ConcurrentHashMap;
import java.lang.InterruptedException;
import java.io.IOException;

public class RilInterface extends Handler {
    static final String LOG_TAG = "I9100OemService";
    static final int EVENT_RIL_AVAILABLE = 1;
    static final int EVENT_RIL_UNAVAILABLE = 2;
    static final int EVENT_ATTACHMENT_COMPLETE = 3;

    private final OEMRequestBuilder mBuilder;
    private CommandsInterface mCI;
    private boolean mAttached = false;
    private Integer mCallIdCounter = new Integer(0);
    private final ConcurrentHashMap<Integer, Object> mLockMap = new ConcurrentHashMap<Integer, Object>();
    private final ConcurrentHashMap<Integer, AsyncResult> mResultMap = new ConcurrentHashMap<Integer, AsyncResult>();
    private RilRequestCompletionThread mCompletionThread;

    public RilInterface() {
        super();

        PhoneProxy proxy = (PhoneProxy) PhoneFactory.getDefaultPhone();

        mBuilder = new OEMRequestBuilder();
        mCI = ((PhoneBase) proxy.getActivePhone()).mCM;
        mCI.registerForAvailable(this, EVENT_RIL_AVAILABLE, null);
        mCI.registerForNotAvailable(this, EVENT_RIL_UNAVAILABLE, null);
        mCompletionThread = new RilRequestCompletionThread(this);
        mCompletionThread.start();
    }

    public void dispose() {
        mCompletionThread.interrupt();
        try {
            mCompletionThread.join();
        } catch(InterruptedException e) {

        }

        if(mCI.getRadioState().isAvailable()) {
            Log.d(LOG_TAG, "Detaching from the RIL");

            byte[] attach = mBuilder.buildAttach(false);

            if(attach != null)
                mCI.invokeOemRilRequestRaw(attach, obtainMessage(EVENT_ATTACHMENT_COMPLETE, 0, 0));
        }

        mCI.unregisterForAvailable(this);
        mCI.unregisterForNotAvailable(this);
    }

    public boolean isAttached() {
        return mAttached;
    }

    public boolean setLoopbackTest(int loopback, int path) {
        return synchronousOemCall(mBuilder.buildSetLoopbackTest(loopback, path), null);
    }

    public boolean setDhaSolution(int mode, int select, String extra) {
        return synchronousOemCall(mBuilder.buildSetDhaSolution(mode, select, extra), null);
    }

    public boolean setTwoMicControl(int device, int report) {
        return synchronousOemCall(mBuilder.buildSetTwoMicControl(device, report), null);
    }

    public boolean getMute(boolean[] muted) {
        OemRequestReply response = new OemRequestReply();

        if(!synchronousOemCall(mBuilder.buildGetMute(), response))
            return false;

        mBuilder.parseGetMute(response, muted);

        return true;
    }

    public boolean setMute(int mute) {
        return synchronousOemCall(mBuilder.buildSetMute(mute), null);
    }

    public boolean setCallRecord(int record) {
        return synchronousOemCall(mBuilder.buildSetCallRecord(record), null);
    }

    public boolean setCallClockSync(int sync) {
        return synchronousOemCall(mBuilder.buildSetCallClockSync(sync), null);
    }

    public boolean setVideoCallClockSync(int sync) {
        return synchronousOemCall(mBuilder.buildSetVideoCallClockSync(sync), null);
    }

    public boolean setCallAudioPath(int path, int extraVolume) {
        return synchronousOemCall(mBuilder.buildSetCallAudioPath(path, extraVolume), null);
    }

    public boolean setCallVolume(int device, int volume) {
        return synchronousOemCall(mBuilder.buildSetCallVolume(device, volume), null);
    }

    public boolean samsungOemRequest(byte[] data, OemRequestReply reply) {
        return synchronousOemCall(mBuilder.buildSamsungOemRequest(data), reply);
    }

    protected boolean synchronousOemCall(byte[] request, OemRequestReply reply) {
        if(request == null) {
            return false;
        }

        int callId;

        synchronized(mCallIdCounter) {
            callId = ++mCallIdCounter;
        }

        Object lock = new Object();
        mLockMap.put(callId, lock);

        try {
            synchronized(lock) {
                mCI.invokeOemRilRequestRaw(request, mCompletionThread.obtainCompletion(callId));

                lock.wait();
            }

        } catch(InterruptedException e) {
            mLockMap.remove(callId);

            return false;
        }

        mLockMap.remove(callId);
        AsyncResult result = mResultMap.get(callId);
        mResultMap.remove(callId);

        if(result.exception != null) {
            Log.d(LOG_TAG, "Synchronous call failed with ", result.exception);

            return false;
        } else {
            if(reply != null) {
                byte[] data = (byte[]) result.result;

                try {
                    reply.write(data);
                } catch(IOException e) {
                    return false;
                }
            }

            return true;
        }
    }

    public void handleMessage(Message msg) {
        switch (msg.what) {
            case EVENT_RIL_AVAILABLE:
                onRilAvailable();

                break;

            case EVENT_RIL_UNAVAILABLE:
                onRilUnavailable();

                break;

            case EVENT_ATTACHMENT_COMPLETE:
                onAttachmentComplete(msg.arg1 != 0, (AsyncResult) msg.obj);

                break;
        }
    }

    public void onCallComplete(int callId, AsyncResult result) {
        Object lock = mLockMap.get(callId);

        if(lock != null) {
            mResultMap.put(callId, result);

            synchronized(lock) {
                lock.notify();
            }
        }
    }

    protected void onRilAvailable() {
        Log.d(LOG_TAG, "The RIL is available");

        byte[] attach = mBuilder.buildAttach(true);

        if(attach != null)
            mCI.invokeOemRilRequestRaw(attach, obtainMessage(EVENT_ATTACHMENT_COMPLETE, 1, 0));
    }

    protected void onRilUnavailable() {
        Log.d(LOG_TAG, "The RIL isn't available");
    }

    protected void onAttachmentComplete(boolean attached, AsyncResult result) {
        if(result.exception != null) {
            Log.d(LOG_TAG, "Attachment failed: ", result.exception);

            mAttached = false;

        } else {
            if(attached)
                Log.d(LOG_TAG, "Attached to the RIL");
            else
                Log.d(LOG_TAG, "Detached from the RIL");

            mAttached = attached;
        }
    }
}
