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

#include "AndroidHAL.h"
#include "SysfsEHCISwitcher.h"
#include "SysfsModemControl.h"
#include "PHONETIPCTransport.h"
#include "AndroidFileSystem.h"

SamsungIPC::IEHCISwitcher *HAL::AndroidHAL::createEHCISwitcher() {
    return new SysfsEHCISwitcher("/sys/devices/platform/s5p-ehci");
}

SamsungIPC::IModemControl *HAL::AndroidHAL::createModemControl() {
    return new SysfsModemControl("/sys/devices/platform/modemctl");
}

SamsungIPC::IIPCTransport *HAL::AndroidHAL::createIPCTransport() {
    return new PHONETIPCTransport("svnet0");
}

SamsungIPC::IFileSystem *HAL::AndroidHAL::createFilesystem() {
    return new AndroidFileSystem("/dev/block/mmcblk0p8");
}
