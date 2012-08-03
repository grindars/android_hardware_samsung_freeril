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

#ifndef __MESSAGE__H___
#define __MESSAGE__H___

#include <stdint.h>

namespace SamsungIPC {
    class IMessageVisitor;

    class Message {
    public:
        virtual ~Message();

        enum ResponseType {
            IPC_CMD_INDI = 1,
            IPC_CMD_RESP = 2,
            IPC_CMD_NOTI = 3
        };

        enum RequestType {
            IPC_CMD_EXEC  = 1,
            IPC_CMD_GET   = 2,
            IPC_CMD_SET   = 3,
            IPC_CMD_CFRM  = 4,
            IPC_CMD_EVENT = 5
        };

        enum MainCommand {
            IPC_PWR_CMD     = 1,
            IPC_CALL_CMD    = 2,
            IPC_SMS_CMD     = 4,
            IPC_SEC_CMD     = 5,
            //IPC_PB_CMD    = 6 - not on wire
            IPC_DISP_CMD    = 7,
            IPC_NET_CMD     = 8,
            IPC_SND_CMD     = 9,
            IPC_MISC_CMD    = 10,
            IPC_SVC_CMD     = 11,
            IPC_SS_CMD      = 12,
            IPC_GPRS_CMD    = 13,
            IPC_SAT_CMD     = 14,
            IPC_CFG_CMD     = 15,
            IPC_IMEI_CMD    = 16,
            IPC_GPS_CMD     = 17,
            //IPC_SAP_CMD   = 18 - not on wire
            IPC_FACTORY_CMD = 19,
            IPC_RFS_CMD     = 21,
            //IPC_GEN_CMD   = 128 - not on wire,

            MAIN_CMD_FIRST  = IPC_PWR_CMD,
            MAIN_CMD_LAST   = IPC_RFS_CMD
        };

        enum SubCommand {
            IPC_PWR_POWER_COMPLETED_NOTIFY         = 1,
            IPC_PWR_PHONE_POWER_OFF                = 2,
            IPC_PWR_PHONE_RESET                    = 3,
            IPC_PWR_LPM_TO_NORMAL_COMPLETED_NOTIFY = 7,

            POWER_CMD_FIRST = IPC_PWR_POWER_COMPLETED_NOTIFY,
            POWER_CMD_LAST  = IPC_PWR_LPM_TO_NORMAL_COMPLETED_NOTIFY,

            IPC_CALL_INCOMING_NOTIFY = 2,
            IPC_CALL_STATUS          = 5,
            IPC_CALL_LIST            = 6,
            IPC_CALL_WAITING         = 9,
            IPC_CALL_TIME            = 13,

            CALL_CMD_FIRST           = IPC_CALL_INCOMING_NOTIFY,
            CALL_CMD_LAST            = IPC_CALL_TIME,

            IPC_SMS_SEND_MSG         = 1,
            IPC_SMS_INCOMING         = 2,
            IPC_SMS_READ_MSG         = 3,
            IPC_SMS_SAVE_MSG         = 4,
            IPC_SMS_DELETE_MSG       = 5,
            IPC_SMS_DELIVER_REPORT   = 6,
            IPC_SMS_DEVICE_READY     = 7,
            IPC_SMS_STORED_MSG_COUNT = 9,
            IPC_SMS_SVC_CENTER_ADDR  = 10,
            IPC_SMS_MEM_STATUS       = 12,
            IPC_SMS_INCOMING_CB      = 13,
            IPC_SMS_CB_CONFIG        = 14,
        };

        struct Header {
            uint16_t length;
            uint8_t mseq;
            uint8_t aseq;
            uint8_t mainCommand;
            uint8_t subCommand;
            uint8_t responseType;
        } __attribute__((packed));

        typedef Message *(*MessageFactory)(const Header &header,
                                           const void *data);

        static Message *create(const Header &header, const void *data);

        virtual void accept(IMessageVisitor *visitor) = 0;

    private:
        static Message *createFromDispatchTable(const MessageFactory table[],
                                                int first, int last, int type,
                                                const Header &header,
                                                const void *data);

        static Message *createPWR(const Header &header, const void *data);
        static Message *createCALL(const Header &header, const void *data);
        static Message *createSMS(const Header &header, const void *data);
        static Message *createSEC(const Header &header, const void *data);
        static Message *createDISP(const Header &header, const void *data);
        static Message *createNET(const Header &header, const void *data);
        static Message *createSND(const Header &header, const void *data);
        static Message *createMISC(const Header &header, const void *data);
        static Message *createSVC(const Header &header, const void *data);
        static Message *createSS(const Header &header, const void *data);
        static Message *createGPRS(const Header &header, const void *data);
        static Message *createSAT(const Header &header, const void *data);
        static Message *createCFG(const Header &header, const void *data);
        static Message *createIMEI(const Header &header, const void *data);
        static Message *createGPS(const Header &header, const void *data);
        static Message *createFACTORY(const Header &header, const void *data);
        static Message *createRFS(const Header &header, const void *data);

        static const MessageFactory m_mainDispatch[MAIN_CMD_LAST -
                                                   MAIN_CMD_FIRST + 1];

        static const MessageFactory m_powerDispatch[POWER_CMD_LAST -
                                                    POWER_CMD_FIRST + 1];
/*
        static const MessageFactory m_callDispatch[CALL_CMD_LAST -
                                                   CALL_CMD_FIRST + 1];*/
    };
}

#endif
