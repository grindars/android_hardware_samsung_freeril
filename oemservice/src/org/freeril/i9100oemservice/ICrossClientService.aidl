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
import org.freeril.i9100oemservice.OemRequestReply;

/*
 * This interface exists solely for compatibility with binary audio and GPS
 * implementations. Please do not depend on it in any way.
 */
interface ICrossClientService {

    /*
     * Audio interface.
     * Allowed users: root, phone, media.
     *
     * See secril-client-compat.h for details.
     */

    boolean setLoopbackTest(
        in int loopback,
        in int path
    );
    boolean setDhaSolution(
        in int mode,
        in int select,
        in String extra
    );
    boolean setTwoMicControl(
        in int device,
        in int report
    );
    boolean getMute(
        inout boolean[] muted
    );
    boolean setMute(
        in int mute
    );
    boolean setCallRecord(
        in int record
    );
    boolean setCallClockSync(
        in int sync
    );
    boolean setVideoCallClockSync(
        in int sync
    );
    boolean setCallAudioPath(
        in int path,
        in int extraVolume
    );
    boolean setCallVolume(
        in int device,
        in int volume
    );

    /*
     * GPS interface.
     * Allowed users: root, phone, system.
     *
     * Returns true on success, false on error.
     * Request will be wrapped into OEM_REQUEST_SAMSUNG_OEM_REQUEST.
     */
    boolean samsungOemRequest(
        in byte[] data,
        out OemRequestReply reply
    );
}
