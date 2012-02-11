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
#include "IIPCSocket.h"
#include "IFileSystem.h"

#include <stdio.h>
#include <unistd.h>
#include <memory>

using namespace SamsungIPC;

SamsungModem::SamsungModem(SamsungIPC::ISamsungIPCHAL *hal) {
    m_ehci = hal->createEHCISwitcher();
    m_modemctl = hal->createModemControl();
    m_ipctransport = hal->createIPCTransport();
    m_filesystem = hal->createFilesystem();
}

SamsungModem::~SamsungModem() {
    delete m_ehci;
    delete m_modemctl;
    delete m_ipctransport;
    delete m_filesystem;
}

void SamsungModem::boot() {
    m_ehci->setRun(true);

    m_modemctl->setState(IModemControl::Reset);
    usleep(100000);
    m_modemctl->setState(IModemControl::On);

    printf("Rebooting modem\n");

    rebootModem();

    printf("Modem rebooted, opening socket\n");

    std::auto_ptr<IIPCSocket> bootSocket(m_ipctransport->createSocket(0));

    unsigned char ack;

    bootSocket->send("ATAT", 4);
    if(bootSocket->recv(&ack, 1) == 0)
        throw TimeoutException("Bootloader greet timeout");

    if(bootSocket->recv(&ack, 1) == 0)
        throw TimeoutException("Chip id read timeout");

    printf("Bootloader ready, loading PSI\n");

    sendPSI(bootSocket.get());

    printf("PSI sent\n");
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

void SamsungModem::sendPSI(IIPCSocket *socket) {
    std::string image = m_filesystem->getFirmware(IFileSystem::PSI);

    psi_header_t header;
    header.header = 0x30;
    header.length = image.length();
    header.trailer = 0xFF;

    socket->send(&header, sizeof(psi_header_t));

    unsigned int offset = 0;

    while(offset < image.length()) {
        ssize_t bytes = socket->send(image.data() + offset, image.length() - offset);

        offset += bytes;
    }

    unsigned char crc = calculateCRC(image), ack;

    socket->send(&crc, 1);

    for(unsigned int i = 0; i < 22; i++) {
        unsigned char ack;
        if(socket->recv(&ack, 1) == 0)
            throw TimeoutException("PSI ACK timeout");
    }

}

unsigned char SamsungModem::calculateCRC(const std::string &data) {
    const unsigned char *ptr = (const unsigned char *) data.data();
    unsigned char crc = 0;
    unsigned int len = data.length();

    while(len--)
        crc ^= *ptr++;

    return crc;
}

