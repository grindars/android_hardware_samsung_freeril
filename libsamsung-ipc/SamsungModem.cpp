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

#include "SamsungModem.h"
#include "ISamsungIPCHAL.h"
#include "IEHCISwitcher.h"
#include "IModemControl.h"
#include "IIPCTransport.h"
#include "Exceptions.h"

#include <stdio.h>
#include <unistd.h>

using namespace SamsungIPC;

SamsungModem::SamsungModem(SamsungIPC::ISamsungIPCHAL *hal) {
    m_ehci = hal->createEHCISwitcher();
    m_modemctl = hal->createModemControl();
    m_ipctransport = hal->createIPCTransport();
}

SamsungModem::~SamsungModem() {
    delete m_ehci;
    delete m_modemctl;
    delete m_ipctransport;
}

void SamsungModem::boot() {
    m_ehci->setRun(true);

    printf("Rebooting modem\n");

    rebootModem();

    printf("Modem rebooted, opening socket.\n");
}

void SamsungModem::rebootModem() {
    int tries = 4;

    m_ehci->setPower(false);

    m_modemctl->setWakeup(true);
    while(!m_modemctl->isWokenUp()) {
        if(--tries == 0) {
            m_modemctl->setWakeup(false);

            throw TimeoutException("Modem wake timeout");
        }

        usleep(500000);
    }

    m_ehci->setPower(true);

    m_modemctl->setState(IModemControl::On);

    tries = 4;

    while(!m_ipctransport->isLinkUp()) {
        if(--tries == 0)
            throw TimeoutException("PHONET link timeout");

        usleep(500000);
    }

    m_ipctransport->setUp(false);
    m_ipctransport->setUp(true);
}
