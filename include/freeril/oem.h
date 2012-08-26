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

#ifndef __FREERIL__OEM__H__
#define __FREERIL__OEM__H__

#include <stdint.h>

namespace FreeRIL {

    enum {
        OEM_SIGNATURE_LOW  = 0x65657246,
        OEM_SIGNATURE_HIGH = 0x214c4952
    };

    /*
     * Header for requests and unsolicited responses.
     */
    struct OemHeader {
        uint32_t signature[2]; // { OEM_SIGNATURE_LOW, OEM_SIGNATURE_HIGH }
        uint32_t request;
    };

    enum OemRequest {
        /*
         * Notify RIL that OEM Service is (not) running.
         *
         * Request:
         *  - int32_t running;
         * Response:
         *  (none)
         */
        OEM_REQUEST_ATTACH_SERVICE = 1,

        /*
         * Set loopback test mode.
         *
         * Request:
         *  - int32_t path
         *  - int32_t loopback
         * Response:
         *  (none)
         */
        OEM_REQUEST_SET_LOOPBACK_TEST,

        /*
         * Set DHA solution.
         *
         * Request:
         *  - int32_t mode
         *  - int32_t select
         *  - UTF     extra
         * Response:
         *  (none)
         */
        OEM_REQUEST_SET_DHA_SOLUTION,

        /*
         * Control dual-microphone processing.
         *
         * Request:
         *  - int32_t param1
         *  - int32_t param2
         * Response:
         *  (none)
         */
        OEM_REQUEST_SET_TWO_MIC_CONTROL,

        /*
         * Get mute status.
         *
         * Request:
         *  (none)
         * Response:
         *  int32_t muted
         */
        OEM_REQUEST_GET_MUTE,

        /*
         * Set mute status.
         *
         * Request:
         *  int32_t mute
         * Response:
         *  (none)
         */
        OEM_REQUEST_SET_MUTE,

        /*
         * Set call recording status.
         *
         * Request:
         *  int32_t record
         * Response:
         *  (none)
         */
        OEM_REQUEST_SET_CALL_RECORD,

        /*
         * Set call clock synchronization.
         *
         * Request:
         *  int32_t sync
         * Response:
         *  (none)
         */
        OEM_REQUEST_SET_CALL_CLOCK_SYNC,

        /*
         * Set video call clock synchronization.
         *
         * Request:
         *  int32_t sync
         * Response:
         *  (none)
         */
        OEM_REQUEST_SET_VIDEO_CALL_CLOCK_SYNC,

        /*
         * Set call audio path.
         *
         * Request:
         *  int32_t path
         *  int32_t extraVolume
         * Response:
         *  (none)
         */
        OEM_REQUEST_SET_CALL_AUDIO_PATH,

        /*
         * Set call volume.
         *
         * Request:
         *  int32_t device
         *  int32_t volume
         * Response:
         *  (none)
         */
        OEM_REQUEST_SET_CALL_VOLUME,

        /*
         * Run Samsung-compatible GPS OEM request.
         *
         * Request:
         *  (varies)
         * Response:
         *  (varies)
         */
        OEM_REQUEST_SAMSUNG_OEM_REQUEST,

        OEM_REQUEST_FIRST = OEM_REQUEST_ATTACH_SERVICE,
        OEM_REQUEST_LAST = OEM_REQUEST_SAMSUNG_OEM_REQUEST
    };

}

#endif
