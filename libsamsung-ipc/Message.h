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
#include <string>

#include "MessageTypes.h"
#include "config.h"

namespace SamsungIPC {
    class DataStream;
    class IUnsolicitedReceiver;

    class Message {
    public:
        typedef void (*CompletionFunction)(Message *reply, void *arg);

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

        struct Header {
            uint16_t length;
            uint8_t mseq;
            uint8_t aseq;
            uint8_t mainCommand;
            uint8_t subCommand;
            uint8_t responseType;
        } __attribute__((packed));

        struct RFSHeader {
            uint32_t length;
            uint8_t type;
            uint8_t sequence;
        } __attribute__((packed));


        virtual ~Message();

        inline void subscribe(CompletionFunction func, void *arg) {
            m_completionFunction = func;
            m_completionArg = arg;
        }

        inline void complete(Message *reply) {
            if(m_completionFunction) {
                m_completionFunction(reply, m_completionArg);
            }
        }

        virtual uint8_t command() const = 0;
        virtual uint8_t subcommand() const = 0;
        virtual RequestType requestType() const = 0;
#if defined(PACKET_INSPECTION)
        virtual std::string inspect() const = 0;
#else
        std::string inspect() const;
#endif
        virtual bool deliver(IUnsolicitedReceiver *receiver) = 0;

        virtual bool readFromStream(DataStream &stream) = 0;
        virtual bool writeToStream(DataStream &stream) = 0;

    private:
        CompletionFunction m_completionFunction;
        void *m_completionArg;
    };
}

template<class T> inline T *message_cast(SamsungIPC::Message *message) {
    if(T::isTypeOf(message))
        return static_cast<T *>(message);
    else
        return 0;
}

#endif
