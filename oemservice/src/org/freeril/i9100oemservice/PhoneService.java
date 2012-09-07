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
import android.content.Context;
import android.util.Log;
import android.content.pm.PackageManager;
import android.Manifest;
import java.util.List;
import java.util.ArrayList;
import android.os.IBinder;
import android.os.RemoteException;
import java.util.Iterator;

public class PhoneService extends IPhoneService.Stub implements IOemUnsolicitedReceiver {
    private class EventHandlerRecord {
        private class DeathRecipient implements IBinder.DeathRecipient {
            private EventHandlerRecord mEventHandlerRecord;

            DeathRecipient(EventHandlerRecord handler) {
                mEventHandlerRecord = handler;
            }

            public void binderDied() {
                mEventHandlerRecord.removeSelf();
            }
        }

        public IEventHandler eventHandler = null;
        public final IBinder.DeathRecipient deathRecipient = new DeathRecipient(this);

        public void removeSelf() {
            eventHandler.asBinder().unlinkToDeath(deathRecipient, 0);

            synchronized(mEventHandlers) {
                mEventHandlers.remove(this);
            }
        }
    };

    private RilInterface mRIL;
    private Context mContext;
    private final List<EventHandlerRecord> mEventHandlers = new ArrayList<EventHandlerRecord>();

    public static final String LOG_TAG = "I9100OemService";

    PhoneService(Context context, RilInterface ril) {
        mContext = context;
        mRIL = ril;
    }

    @Override
    public boolean enterServiceMode(int modeType, int subType) {
        ensureSafe();

        return mRIL.enterServiceMode(modeType, subType);
    }

    @Override
    public boolean exitServiceMode(int modeType) {
        ensureSafe();

        return mRIL.exitServiceMode(modeType);
    }

    @Override
    public boolean sendServiceKeyCode(int keyCode) {
        ensureSafe();

        return mRIL.sendServiceKeyCode(keyCode);
    }

    @Override
    public void registerEventHandler(IEventHandler handler) {
        ensureSafe();

        EventHandlerRecord record = new EventHandlerRecord();
        record.eventHandler = handler;

        try {
            handler.asBinder().linkToDeath(record.deathRecipient, 0);
        } catch(RemoteException e) {
            return;
        }

        synchronized(mEventHandlers) {
            mEventHandlers.add(record);
        }
    }

    @Override
    public void unregisterEventHandler(IEventHandler handler) {
        ensureSafe();

        synchronized(mEventHandlers) {
            Iterator<EventHandlerRecord> it = mEventHandlers.iterator();

            while(it.hasNext()) {
                EventHandlerRecord record = it.next();

                if(record.eventHandler == handler) {
                    record.removeSelf();
                }
            }
        }
    }

    private void ensureSafe() {
        if(mContext.checkCallingPermission("org.freeril.i9100oemservice.CALL_PHONE_SERVICE") != PackageManager.PERMISSION_GRANTED) {
            Log.w(LOG_TAG, "Denied access to PhoneService for " + getCallingPid());

            throw new SecurityException("Access denied");
        }
    }

    public void serviceCompleted() {
        synchronized(mEventHandlers) {
            Iterator<EventHandlerRecord> it = mEventHandlers.iterator();

            while(it.hasNext()) {
                EventHandlerRecord record = it.next();

                try {
                    record.eventHandler.handleServiceCompleted();
                } catch(RemoteException e) {
                    record.removeSelf();
                }
            }
        }
    }

    public void serviceDisplay(List<String> lines) {
        synchronized(mEventHandlers) {
            Iterator<EventHandlerRecord> it = mEventHandlers.iterator();

            while(it.hasNext()) {
                EventHandlerRecord record = it.next();

                try {
                    record.eventHandler.handleServiceDisplay(lines);
                } catch(RemoteException e) {
                    record.removeSelf();
                }
            }
        }
    }
}
