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

    bool handleAttachService(bool attach);

    bool handleSetLoopbackTest(int path, int loopback);
    bool handleSetDhaSolution(int mode, int select,
                              const std::string &extra);
    bool handleSetTwoMicControl(int param1, int param2);
    bool handleGetMute(bool *muted);
    bool handleSetMute(bool muted);
    bool handleSetCallRecord(int record);
    bool handleSetCallClockSync(int sync);
    bool handleSetVideoCallClockSync(int sync);
    bool handleSetCallAudioPath(int path, int extraVolume);
    bool handleSetCallVolume(int device, int volume);

    bool handleSamsungOemRequest(android::Parcel &request, android::Parcel &response);

    bool handleEnterServiceMode(int modeType, int subType);
    bool handleExitServiceMode(int modeType);
    bool handleSendServiceKeyCode(int keyCode);

private:
    RIL *m_ril;
};

#endif
