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

#include <stdint.h>
#include <string>

namespace SamsungIPC {
    class ISamsungIPCHAL;
    class IEHCISwitcher;
    class IModemControl;
    class IIPCTransport;
    class IFileSystem;
    class IIPCSocket;

    class SamsungModem {
    public:
        SamsungModem(ISamsungIPCHAL *hal);
        virtual ~SamsungModem();

        void boot();

    private:
        enum {
            SetPortConf        = 0x86,

            ReqSecStart        = 0x204,
            ReqSecEnd          = 0x205,
            ReqForceHwReset    = 0x208,

            ReqFlashSetAddress = 0x802,
            ReqFlashWriteBlock = 0x804
        };

        typedef struct {
            uint8_t indicate;
            uint16_t length;
            uint8_t dummy;
        } __attribute__((packed)) psi_header_t;

        typedef struct {
            uint8_t unknown[76];
        } __attribute__((packed)) boot_info_t;

        typedef struct {
            uint16_t check;
            uint16_t cmd;
            uint32_t data_size;
        } bootloader_cmd_t;

        enum FlashImageType {
            FirmwareFlashImage  = 2,
            NVDataFlashImage    = 3
        };

        void rebootModem(bool soft = false);

        void sendPSI(IIPCSocket *socket);
        void sendEBL(IIPCSocket *socket);
        void readBootInfo(IIPCSocket *socket);
        void sendSecureImage(IIPCSocket *socket);

        void loadFlashImage(IIPCSocket *socket, uint32_t address,
            std::string image);
        void bootloaderCommand(IIPCSocket *socket, uint16_t cmd,
                               const void *data, size_t data_size);
        void expectAck(IIPCSocket *socket, const unsigned char *data, size_t size);

        static unsigned char calculateCRC(const std::string &data);

        IEHCISwitcher *m_ehci;
        IModemControl *m_modemctl;
        IIPCTransport *m_ipctransport;
        IFileSystem *m_filesystem;
    };
}

#endif
