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

#include <fcntl.h>

#include "SysfsEHCISwitcher.h"
#include "NativeFile.h"

RIL::SysfsEHCISwitcher::SysfsEHCISwitcher(const std::string &device) : SysfsControlledDevice(device) {

}

RIL::SysfsEHCISwitcher::~SysfsEHCISwitcher() {

}

bool RIL::SysfsEHCISwitcher::run() const {
    return read("runtime") == "on";
}

bool RIL::SysfsEHCISwitcher::power() const {
    return read("power") == "on";
}

void RIL::SysfsEHCISwitcher::setRun(bool run) {
    write("runtime", run ? "on" : "off");
}

void RIL::SysfsEHCISwitcher::setPower(bool power) {
    write("power", power ? "on" : "off");
}
