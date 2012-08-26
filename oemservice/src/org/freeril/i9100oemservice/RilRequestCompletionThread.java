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
import android.os.HandlerThread;
import android.os.Process;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.os.AsyncResult;

class RilRequestCompletionThread extends HandlerThread {
    private Handler mHandler = null;
    private RilInterface mInterface;
    static final int EVENT_SYNCHRONOUS_CALL_COMPLETE = 1;

    public RilRequestCompletionThread(RilInterface rilInterface) {
        /*
         * Why AUDIO? Because most requests are from it.
         */

        super("RilRequestCompletionThread", Process.THREAD_PRIORITY_URGENT_AUDIO);

        mInterface = rilInterface;
    }

    public Message obtainCompletion(int callId) {
        return mHandler.obtainMessage(EVENT_SYNCHRONOUS_CALL_COMPLETE, callId, 0);
    }

    @Override
    protected void onLooperPrepared() {
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                if(msg.what == EVENT_SYNCHRONOUS_CALL_COMPLETE)
                    mInterface.onCallComplete(msg.arg1, (AsyncResult) msg.obj);
            }
        };
    }
}
