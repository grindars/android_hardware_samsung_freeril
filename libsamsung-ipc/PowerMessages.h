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

#ifndef __POWER_MESSAGES__H__
#define __POWER_MESSAGES__H__

#include "Message.h"

namespace SamsungIPC {
namespace Messages {
    class PowerCompleted: public Message {
    public:

        virtual void accept(IMessageVisitor *visitor);

        static Message *parse(const Header &header, const void *data);
    };

    class PhonePowerOff: public Message {
    public:
        enum PowerOffReason {
            LowBattery = 0
        };

        inline PowerOffReason reason() const { return m_reason; }
        inline void setReason(PowerOffReason reason) { m_reason = reason; }

        virtual void accept(IMessageVisitor *visitor);

        static Message *parse(const Header &header, const void *data);

    protected:
        struct Data {
            uint8_t reason;
        };

    private:
        PowerOffReason m_reason;
    };

    class PhoneReset: public Message {
    public:
        virtual void accept(IMessageVisitor *visitor);

        static Message *parse(const Header &header, const void *data);
    };

    class LPMToNormalCompleted: public Message {
    public:
        enum Type {
            CallOemInit = 2
        };

        inline Type type() const { return m_type; }
        inline void setType(Type type) { m_type = type; }

        virtual void accept(IMessageVisitor *visitor);

        static Message *parse(const Header &header, const void *data);

    protected:
        struct Data {
            uint8_t type;
        };

    private:
        Type m_type;
    };
}
}

#endif
