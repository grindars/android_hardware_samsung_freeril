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

#ifndef __OEM_UNSOLICITED_BUILDER__H__
#define __OEM_UNSOLICITED_BUILDER__H__

#include <string>
#include <vector>

class RIL;

namespace android {
    class Parcel;
}

class OemUnsolicitedBuilder {
public:
    OemUnsolicitedBuilder(RIL *ril);

    void notifyServiceCompleted();
    void notifyServiceDisplay(const std::vector<std::string> &display);

private:
    void beginUnsolicited(int type, android::Parcel **parcel);
    void endUnsolicited(android::Parcel *parcel);

    RIL *m_ril;
};

#endif
