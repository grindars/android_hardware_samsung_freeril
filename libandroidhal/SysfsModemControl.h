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

#ifndef __ANDROIDHAL__SYSFSMODEMCONTROL__H__
#define __ANDROIDHAL__SYSFSMODEMCONTROL__H__

#include <IModemControl.h>
#include <string>
#include "SysfsControlledDevice.h"

namespace HAL {
    class SysfsModemControl: public SamsungIPC::IModemControl,
                             public SysfsControlledDevice {
    public:
        SysfsModemControl(const std::string &device);
        virtual ~SysfsModemControl();

        virtual bool isWokenUp() const;
        virtual void setWakeup(bool wake);

        virtual SamsungIPC::IModemControl::State state() const;
        virtual void setState(SamsungIPC::IModemControl::State state);

    private:
        //std::string m_path;
        SamsungIPC::IModemControl::State m_state;
    };
}

#endif
