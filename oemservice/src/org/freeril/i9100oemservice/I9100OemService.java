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

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.content.ServiceConnection;
import android.content.Context;
import android.content.ComponentName;
import android.os.RemoteException;

public class I9100OemService extends Service {
    private CrossClientService mCrossClientService;
    private PhoneService mPhoneService;
    private RilInterface mRIL;

    private IRegistrationInterface mRegistrationService;
    private final ServiceConnection mRegistrationConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            mRegistrationService = IRegistrationInterface.Stub.asInterface(service);
            registerServices();
        }

        public void onServiceDisconnected(ComponentName className) {
            serviceRegistrationFailed();
        }
    };

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        /*
         * Normal bindService-based lifecycle management isn't available for
         * native clients.
         *
         * Also, we don't have access to ServiceManager.addService.
         */

        Intent registrationServiceIntent = new Intent(this, RegistrationService.class);
        bindService(registrationServiceIntent, mRegistrationConnection,
                    Context.BIND_AUTO_CREATE | Context.BIND_IMPORTANT);

        return START_STICKY;
    }

    @Override
    public void onCreate() {
        super.onCreate();

        mRIL = new RilInterface();
        mCrossClientService = new CrossClientService(mRIL);
        mPhoneService = new PhoneService(this, mRIL);
        mRIL.setUnsolicitedReceiver(mPhoneService);
    }

    @Override
    public void onDestroy() {
        mRIL.setUnsolicitedReceiver(null);
        mRIL.dispose();

        super.onDestroy();
    }

    private void registerServices() {
        try {
            mRegistrationService.registerService("org.freeril.i9100oemservice.CrossClientService", mCrossClientService);
            mRegistrationService.registerService("org.freeril.i9100oemservice.PhoneService", mPhoneService);
        } catch(android.os.RemoteException e) {
            stopSelf();
        } finally {
            mRegistrationService = null;
            unbindService(mRegistrationConnection);
        }
    }

    private void serviceRegistrationFailed() {
        stopSelf();
    }
}
