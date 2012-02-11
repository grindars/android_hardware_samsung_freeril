/*
 * Free HAL implementation for Samsung Android-based smartphones.
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

HAL::SysfsEHCISwitcher::SysfsEHCISwitcher(const std::string &device) : SysfsControlledDevice(device) {

}

HAL::SysfsEHCISwitcher::~SysfsEHCISwitcher() {

}

bool HAL::SysfsEHCISwitcher::run() const {
    return read("ehci_runtime") == "on\n";
}

bool HAL::SysfsEHCISwitcher::power() const {
    return read("ehci_power") == "EHCI Power on\n";
}

void HAL::SysfsEHCISwitcher::setRun(bool run) {
    write("ehci_runtime", run ? "on" : "off");
}

void HAL::SysfsEHCISwitcher::setPower(bool power) {
    write("ehci_power", power ? "1" : "0");
}
