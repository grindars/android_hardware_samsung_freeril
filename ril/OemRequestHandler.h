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

#ifndef __OEM_REQUEST_HANDLER__H__
#define __OEM_REQUEST_HANDLER__H__

#include <string>

namespace android {
    class Parcel;
}

class RIL;

class OemRequestHandler {
public:
    OemRequestHandler(RIL *ril);

    void handleAttachService(bool attach);

    void handleSetLoopbackTest(int path, int loopback);
    void handleSetDhaSolution(int mode, int select,
                              const std::string &extra);
    void handleSetTwoMicControl(int param1, int param2);
    bool handleGetMute();
    void handleSetMute(bool muted);
    void handleSetCallRecord(int record);
    void handleSetCallClockSync(int sync);
    void handleSetVideoCallClockSync(int sync);
    void handleSetCallAudioPath(int path, int extraVolume);
    void handleSetCallVolume(int device, int volume);

    bool handleSamsungOemRequest(android::Parcel &request, android::Parcel &response);

private:
    RIL *m_ril;
};

#endif
