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
#include "Utilities.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

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

    unsigned char ack[2];

    bootSocket->send("ATAT", 4);
    if(bootSocket->recv(ack, 1) == 0)
        throw TimeoutException("Bootloader greet timeout");

    if(bootSocket->recv(ack, 1) == 0)
        throw TimeoutException("Chip id read timeout");

    printf("Bootloader ready, loading PSI\n");

    sendPSI(bootSocket.get());

    printf("PSI sent\n");

    ack[0] = 0x00;
    ack[1] = 0xAA;
    expectAck(bootSocket.get(), ack, 2);

    printf("PSI ready, loading EBL\n");

    sendEBL(bootSocket.get());

    printf("EBL sent and ready, configuring modem\n");

    readBootInfo(bootSocket.get());

    printf("Port configured, sending secure image\n");

    sendSecureImage(bootSocket.get());

    printf("Secure image sent, rebooting modem\n");

    bootSocket->close();

    rebootModem();

    printf("Modem boot sequence complete\n");
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
    header.indicate = 0x30;
    header.length = image.length();
    header.dummy = 0xFF;

    socket->send(&header, sizeof(psi_header_t));

    unsigned int offset = 0;

    while(offset < image.length()) {
        ssize_t bytes = socket->send(image.data() + offset, image.length() - offset);

        offset += bytes;
    }

    unsigned char crc = calculateCRC(image);

    socket->send(&crc, 1);

    for(unsigned int i = 0; i < 22; i++) {
        unsigned char ack;
        if(socket->recv(&ack, 1) == 0)
            throw TimeoutException("PSI ACK timeout");
    }

    unsigned char valid_ack = 0x01;
    expectAck(socket, &valid_ack, 1);
    expectAck(socket, &valid_ack, 1);
}

void SamsungModem::sendEBL(IIPCSocket *socket) {
    std::string image = m_filesystem->getFirmware(IFileSystem::EBL);

    unsigned char valid_ack[2] = { 0xCC, 0xCC };

    unsigned int length = image.length();
    socket->send(&length, sizeof(unsigned int));
    expectAck(socket, valid_ack, 2);

    unsigned int offset = 0;

    while(offset < image.length()) {
        size_t chunk = std::min<size_t>(image.length() - offset, 32768);

        ssize_t bytes = socket->send(image.data() + offset, chunk);

        offset += bytes;
    }

    unsigned char crc = calculateCRC(image);

    socket->send(&crc, 1);

    valid_ack[0] = 0x51;
    valid_ack[1] = 0xA5;
    expectAck(socket, valid_ack, 2);
}


void SamsungModem::readBootInfo(IIPCSocket *socket) {
    boot_info_t info;
    if(socket->recv(&info, sizeof(boot_info_t)) != sizeof(boot_info_t))
        throw TimeoutException("Bootloader information timeout");

    printf("Bootloader information dump:\n");
    SamsungIPC::dump(&info, sizeof(boot_info_t));

    bootloaderCommand(socket, SetPortConf, &info, sizeof(boot_info_t));
}

void SamsungModem::bootloaderCommand(IIPCSocket *socket,
                                   uint16_t cmd,
                                   const void *data,
                                   size_t data_size) {

    const unsigned char *ptr = (unsigned char *) data;
    uint16_t acc = (data_size & 0xFFFF) + cmd;

    for(size_t offset = 0; offset < data_size; offset++)
        acc += ptr[offset];

    bootloader_cmd_t header = { acc, cmd, data_size }, reply;

    unsigned int size;

    if(cmd == SetPortConf) {
        size = 0x800 + sizeof(bootloader_cmd_t);
    } else {
        size = 0x4000 + sizeof(bootloader_cmd_t);
    }

    std::auto_ptr<char> buf(new char[size]);
    memset(buf.get(), 0, size);
    memcpy(buf.get(), &header, sizeof(bootloader_cmd_t));
    memcpy(buf.get() + 8, data, data_size);

    printf("Generated command of size %u:\n", size);
    SamsungIPC::dump(buf.get(), 32);

    socket->send(buf.get(), size);

    if(cmd != ReqForceHwReset && cmd != ReqFlashWriteBlock) {
        printf("Reading reply\n");

        if(socket->recv(&reply, sizeof(bootloader_cmd_t)) !=
            sizeof(bootloader_cmd_t))
            throw TimeoutException("Bootloader command reply timeout");

        printf("Bootloader command reply header:\n");
        SamsungIPC::dump(&reply, sizeof(bootloader_cmd_t));

        if(reply.cmd != cmd)
            throw CommunicationErrorException(
                "Bootloader reply command "
                "doesn't match with request command");

        size -= sizeof(bootloader_cmd_t);

        std::auto_ptr<char> reply_buf(new char[size]);
        if(socket->recv(reply_buf.get(), size) != (ssize_t) size)
            throw TimeoutException("Bootloader command reply data timeout");

        printf("Bootloader reply data:\n");
        SamsungIPC::dump(reply_buf.get(), size);
    }
}

void SamsungModem::sendSecureImage(IIPCSocket *socket) {
    uint16_t end_magic   = 0x0000;
    uint32_t reset_magic = 0x00111001;

    std::string secure = m_filesystem->getFirmware(IFileSystem::SecureImage);
    std::string firmware = m_filesystem->getFirmware(IFileSystem::Firmware);
    std::string nvdata;

    try {
        nvdata = m_filesystem->readNVData();
    } catch(std::exception &e) {
        fprintf(stderr, "SamsungModem::sendSecureImage: NVData read failed: %s."
                        " Performing recovery.\n", e.what());

        nvdata = m_filesystem->getFirmware(IFileSystem::DefaultNVData);
        m_filesystem->writeNVData(nvdata);
    }

    bootloaderCommand(socket, ReqSecStart, secure.data(), secure.length());

    loadFlashImage(socket, 0x60300000, firmware);
    loadFlashImage(socket, 0x60e80000, nvdata);

    bootloaderCommand(socket, ReqSecEnd, &end_magic, 2);
    bootloaderCommand(socket, ReqForceHwReset, &reset_magic, 4);
}

void SamsungModem::loadFlashImage(IIPCSocket *socket, uint32_t address,
    std::string image) {

    bootloaderCommand(socket, ReqFlashSetAddress, &address, 4);

    unsigned int offset = 0;

    while(offset < image.length()) {
        size_t chunk = std::min<size_t>(image.length() - offset, 16384);

        bootloaderCommand(socket, ReqFlashWriteBlock, image.data() + offset,
                          chunk);

        offset += chunk;
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

void SamsungModem::expectAck(IIPCSocket *socket, const unsigned char *data,
                             size_t size) {

    std::auto_ptr<char> buf(new char[size]);

    ssize_t bytes = socket->recv(buf.get(), size);
    if(bytes < (ssize_t) size)
        throw TimeoutException("ACK timeout or incomplete ACK");

    printf("Received ACK:\n");
    dump(buf.get(), size);

    if(memcmp(buf.get(), data, size) != 0)
        throw TimeoutException("Valid ACK timeout");
}

