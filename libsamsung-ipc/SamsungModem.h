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

#ifndef __SAMSUNGIPC__SAMSUNGMODEM__H__
#define __SAMSUNGIPC__SAMSUNGMODEM__H__

namespace SamsungIPC {
    class ISamsungIPCHAL;
    class IEHCISwitcher;
    class IModemControl;
    class IIPCTransport;

    class SamsungModem {
    public:
        SamsungModem(ISamsungIPCHAL *hal);
        virtual ~SamsungModem();

        void boot();

    private:
        void rebootModem();

        IEHCISwitcher *m_ehci;
        IModemControl *m_modemctl;
        IIPCTransport *m_ipctransport;
    };
}

#endif
