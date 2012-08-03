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

#include "PowerMessages.h"
#include "IMessageVisitor.h"

using namespace SamsungIPC;
using namespace Messages;

Message *PowerCompleted::parse(const Header &header, const void *data) {
    (void) data;

    if(header.length != sizeof(Header))
        return NULL;

    return new PowerCompleted;
}

void PowerCompleted::accept(IMessageVisitor *visitor) {
    visitor->visit(this);
}

Message *PhonePowerOff::parse(const Header &header, const void *data) {
    if(header.length != sizeof(Header) + sizeof(Data))
        return NULL;

    const Data *msg_data = (const Data *) data;

    PhonePowerOff *msg = new PhonePowerOff;
    msg->setReason((PowerOffReason) msg_data->reason);

    return msg;
}

void PhonePowerOff::accept(IMessageVisitor *visitor) {
    visitor->visit(this);
}

Message *PhoneReset::parse(const Header &header, const void *data) {
    (void) data;

    if(header.length != sizeof(Header))
        return NULL;

    return new PhoneReset;
}

void PhoneReset::accept(IMessageVisitor *visitor) {
    visitor->visit(this);
}

Message *LPMToNormalCompleted::parse(const Header &header, const void *data) {
    if(header.length != sizeof(Header) + sizeof(Data))
        return NULL;

    const Data *msg_data = (const Data *) data;

    LPMToNormalCompleted *msg = new LPMToNormalCompleted;
    msg->setType((Type) msg_data->type);

    return msg;
}

void LPMToNormalCompleted::accept(IMessageVisitor *visitor) {
    visitor->visit(this);
}
