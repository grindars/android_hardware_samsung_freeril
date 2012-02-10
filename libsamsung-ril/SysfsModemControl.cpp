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

#include "SysfsModemControl.h"

RIL::SysfsModemControl::SysfsModemControl(const std::string &device) :
    SysfsControlledDevice(device), m_state(SamsungIPC::IModemControl::Off) {

}

RIL::SysfsModemControl::~SysfsModemControl() {

}

bool RIL::SysfsModemControl::isWokenUp() const {
    return read("wakeup") == "1";
}

void RIL::SysfsModemControl::setWakeup(bool wake) {
    write("wakeup", wake ? "1" : "0");
}

SamsungIPC::IModemControl::State RIL::SysfsModemControl::state() const {
    return m_state;
}

void RIL::SysfsModemControl::setState(SamsungIPC::IModemControl::State state) {
    switch(state) {
    case SamsungIPC::IModemControl::On:
        write("control", "on");

        break;

    case SamsungIPC::IModemControl::Off:
        write("control", "off");

        break;

    case SamsungIPC::IModemControl::Reset:
        write("control", "reset");

        break;
    }

    m_state = state;
}
