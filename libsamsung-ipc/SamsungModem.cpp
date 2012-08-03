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
#include "IIPCTransport.h"
#include "IIPCSocket.h"
#include "IFileSystem.h"
#include "Utilities.h"
#include "IProgressCallback.h"
#include "IPCWorkerThread.h"
#include "IPCSocketHandler.h"
#include "RFSSocketHandler.h"
#include "Log.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <memory>
#include <vector>

using namespace SamsungIPC;

SamsungModem::SamsungModem(SamsungIPC::ISamsungIPCHAL *hal) {
    m_ipctransport = hal->createIPCTransport();
    m_filesystem = hal->createFilesystem();
}

SamsungModem::~SamsungModem() {
    if(m_worker) {
        Log::debug("Requesting worker stop\n");

        m_worker->wait();
        delete m_worker;
    }

    delete m_ipctransport;
    delete m_filesystem;
}

bool SamsungModem::boot() {
    uint32_t reset_magic = 0x00111001;

    std::auto_ptr<IIPCSocket> bootSocket(m_ipctransport->createSocket(IIPCTransport::Boot));
    if(!m_ipctransport->connect()) {
        Log::error("Modem is not connected.");

        return false;
    }

    if(!sendPSI(bootSocket.get()))
        return false;

    if(!sendEBL(bootSocket.get()))
        return false;

    if(!readBootInfo(bootSocket.get()))
        return false;

    if(!sendSecureImage(bootSocket.get()))
        return false;

    if(!bootloaderCommand(bootSocket.get(), ReqForceHwReset, &reset_magic, 4))
        return false;

    bootSocket->close();
    if(!m_ipctransport->redetect()) {
        Log::error("Modem did not return from reset.");

        return false;
    }

    return true;
}

bool SamsungModem::dump(std::ostream &stream, IProgressCallback *progress) {
    uint32_t upload_key = 0xDEADDEAD;
    unsigned char error_ack[4] = { 'P', 'A', 'S', 'S' };
    size_t total = 0, expected_total = 50 * 1024 * 1024;

    std::auto_ptr<IIPCSocket> bootSocket(m_ipctransport->createSocket(IIPCTransport::Boot));
    m_ipctransport->connect();

    if(!sendPSI(bootSocket.get()))
        return false;

    bootSocket->send(&upload_key, sizeof(upload_key));

    std::vector<char> buf(16384);

    if(bootSocket->recv(&buf[0], 150) != 150) {
        Log::error("Error message timeout");

        return false;
    }

    stream.write(&buf[0], 150);
    if(stream.bad())
        Log::panicErrno("stream.write");

    bootSocket->send(error_ack, sizeof(error_ack));

    if(progress)
        progress->progress(total, expected_total);

    int bytes;
    do {
        bytes = bootSocket->recv(&buf[0], 16384);

        stream.write(&buf[0], bytes);
        if(stream.bad())
            Log::panicErrno("stream.write");

        total += bytes;
        if(progress)
            progress->progress(total / 16384, expected_total / 16384);
    } while(bytes != 0);

    bootSocket->close();

    return true;
}

bool SamsungModem::sendPSI(IIPCSocket *socket) {
    std::vector<char> image;

    if(!m_filesystem->getFirmware(IFileSystem::PSI, image)) {
        Log::error("PSI is not available");

        return false;
    }

    unsigned char ack[2];

    socket->send("ATAT", 4);
    if(socket->recv(ack, 1) == 0) {
        Log::error("Bootloader greet timeout");

        return false;
    }

    if(socket->recv(ack, 1) == 0) {
        Log::error("Chip id read timeout");

        return false;
    }

    psi_header_t header;
    header.indicate = 0x30;
    header.length = image.size();
    header.dummy = 0xFF;

    socket->send(&header, sizeof(psi_header_t));

    unsigned int offset = 0;

    while(offset < image.size()) {
        ssize_t bytes = socket->send(&image[offset], image.size() - offset);

        offset += bytes;
    }

    unsigned char crc = calculateCRC(image);

    socket->send(&crc, 1);

    for(unsigned int i = 0; i < 22; i++) {
        if(socket->recv(ack, 1) == 0) {
            Log::error("PSI ack timeout");

            return false;
        }
    }

    unsigned char valid_ack = 0x01;
    if(!expectAck(socket, &valid_ack, 1)) {
        Log::error("PSI ack timeout");

        return false;
    }

    if(!expectAck(socket, &valid_ack, 1)) {
        Log::error("PSI ack timeout");

        return false;
    }

    ack[0] = 0x00;
    ack[1] = 0xAA;
    if(!expectAck(socket, ack, 2)) {
        Log::error("PSI ack timeout");

        return false;
    }

    return true;
}

bool SamsungModem::sendEBL(IIPCSocket *socket) {
    std::vector<char> image;

    if(!m_filesystem->getFirmware(IFileSystem::EBL, image)) {
        Log::error("EBL is not available");

        return false;
    }

    unsigned char valid_ack[2] = { 0xCC, 0xCC };

    unsigned int length = image.size();
    socket->send(&length, sizeof(unsigned int));
    if(!expectAck(socket, valid_ack, 2)) {
        Log::error("EBL length ack timeout");

        return false;
    }

    unsigned int offset = 0;

    while(offset < image.size()) {
        size_t chunk = std::min<size_t>(image.size() - offset, 32768);

        ssize_t bytes = socket->send(&image[offset], chunk);

        offset += bytes;
    }

    unsigned char crc = calculateCRC(image);

    socket->send(&crc, 1);

    valid_ack[0] = 0x51;
    valid_ack[1] = 0xA5;
    if(!expectAck(socket, valid_ack, 2)) {
        Log::error("EBL ack timeout");

        return false;
    }

    return true;
}


bool SamsungModem::readBootInfo(IIPCSocket *socket) {
    boot_info_t info;
    if(socket->recv(&info, sizeof(boot_info_t)) != sizeof(boot_info_t)) {
        Log::error("Bootloader information timeout");

        return false;
    }

    return bootloaderCommand(socket, SetPortConf, &info, sizeof(boot_info_t));
}

bool SamsungModem::bootloaderCommand(IIPCSocket *socket,
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

    std::vector<char> buf(size, 0);
    memcpy(&buf[0], &header, sizeof(bootloader_cmd_t));
    memcpy(&buf[8], data, data_size);

    socket->send(&buf[0], size);

    if(cmd != ReqForceHwReset && cmd != ReqFlashWriteBlock) {

        if(socket->recv(&reply, sizeof(bootloader_cmd_t)) !=
            sizeof(bootloader_cmd_t)) {
            Log::error("Bootloader command reply timeout");

            return false;
        }

        if(reply.cmd != cmd) {
            Log::error("Bootloader reply command doesn't match with request command");

            return false;
        }

        size -= sizeof(bootloader_cmd_t);

        std::vector<char> reply_buf(size);
        if(socket->recv(&reply_buf[0], size) != (ssize_t) size) {
            Log::error("Bootloader command reply data timeout");

            return false;
        }
    }

    return true;
}

bool SamsungModem::sendSecureImage(IIPCSocket *socket) {
    uint16_t end_magic   = 0x0000;

    std::vector<char> secure, firmware, nvdata;

    if(!m_filesystem->getFirmware(IFileSystem::SecureImage, secure)) {
        Log::error("Secure image is not available");

        return false;
    }

    if(!m_filesystem->getFirmware(IFileSystem::Firmware, firmware)) {
        Log::error("Firmware is not available");

        return false;
    }

    if(!m_filesystem->readNVData(nvdata)) {
        Log::error("SamsungModem::sendSecureImage: NVData read failed. Performing recovery.");

        if(!m_filesystem->getFirmware(IFileSystem::DefaultNVData, nvdata)) {
            Log::error("Default nvdata image is unavailable too.");

            return false;
        }

        if(!m_filesystem->writeNVData(nvdata)) {
            Log::error("nvdata writeback failed");
        }
    }

    if(!bootloaderCommand(socket, ReqSecStart, &secure[0], secure.size()))
        return false;

    if(!loadFlashImage(socket, 0x60300000, firmware))
        return false;

    if(!loadFlashImage(socket, 0x60e80000, nvdata))
        return false;

    return bootloaderCommand(socket, ReqSecEnd, &end_magic, 2);
}

bool SamsungModem::loadFlashImage(IIPCSocket *socket, uint32_t address,
    const std::vector<char> &image) {

    if(!bootloaderCommand(socket, ReqFlashSetAddress, &address, 4))
        return false;

    unsigned int offset = 0;

    while(offset < image.size()) {
        size_t chunk = std::min<size_t>(image.size() - offset, 16384);

        if(!bootloaderCommand(socket, ReqFlashWriteBlock, &image[offset],
                          chunk))
            return false;

        offset += chunk;
    }

    return true;
}

unsigned char SamsungModem::calculateCRC(const std::vector<char> &data) {
    const unsigned char *ptr = (const unsigned char *) &data[0];
    unsigned char crc = 0;
    unsigned int len = data.size();

    while(len--)
        crc ^= *ptr++;

    return crc;
}

bool SamsungModem::expectAck(IIPCSocket *socket, const unsigned char *data,
                             size_t size) {

    std::vector<char> buf(size);

    ssize_t bytes = socket->recv(&buf[0], size);
    if(bytes < (ssize_t) size)
        return false;

    if(memcmp(&buf[0], data, size) != 0)
        return false;

    return true;
}

void SamsungModem::initialize() {
    m_worker = new IPCWorkerThread();

    m_worker->addHandler(new IPCSocketHandler(m_ipctransport->createSocket(IIPCTransport::IPC)));
//    m_worker->addHandler(new RFSSocketHandler(m_ipctransport->createSocket(IIPCTransport::RFS)));

    m_worker->start();
}
