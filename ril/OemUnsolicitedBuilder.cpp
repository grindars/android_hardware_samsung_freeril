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

#include <freeril/oem.h>
#include <binder/Parcel.h>
#include <utils/String16.h>

#include "OemUnsolicitedBuilder.h"
#include "RIL.h"

using namespace android;
using namespace FreeRIL;

OemUnsolicitedBuilder::OemUnsolicitedBuilder(RIL *ril) : m_ril(ril) {

}

void OemUnsolicitedBuilder::notifyServiceCompleted() {
    Parcel *parcel;

    beginUnsolicited(OEM_UNSOLICITED_SERVICE_COMPLETED, &parcel);
    endUnsolicited(parcel);
}

void OemUnsolicitedBuilder::notifyServiceDisplay(const std::vector<std::string> &display) {
    Parcel *parcel;
    beginUnsolicited(OEM_UNSOLICITED_SERVICE_DISPLAY, &parcel);

    parcel->writeInt32(display.size());
    for(std::vector<std::string>::const_iterator it = display.begin(), end = display.end(); it != end; it++)
        parcel->writeString16(String16((*it).c_str()));


    endUnsolicited(parcel);
}

void OemUnsolicitedBuilder::beginUnsolicited(int type, Parcel **parcel) {
    *parcel = new Parcel;

    (*parcel)->writeInt32(OEM_SIGNATURE_LOW);
    (*parcel)->writeInt32(OEM_SIGNATURE_HIGH);
    (*parcel)->writeInt32(type);
}

void OemUnsolicitedBuilder::endUnsolicited(android::Parcel *parcel) {
    m_ril->unsolicited(RIL_UNSOL_OEM_HOOK_RAW, parcel->data(), parcel->dataSize());

    delete parcel;
}
