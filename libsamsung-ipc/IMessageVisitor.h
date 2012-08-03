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

#ifndef __IMESSAGE_VISITOR__H__
#define __IMESSAGE_VISITOR__H__

namespace SamsungIPC {

namespace Messages {
    class PowerCompleted;
    class PhonePowerOff;
    class PhoneReset;
    class LPMToNormalCompleted;
}

class IMessageVisitor {
public:
    virtual ~IMessageVisitor() {}
    
    virtual void visit(Messages::PowerCompleted *msg) = 0;
    virtual void visit(Messages::PhonePowerOff *msg) = 0;
    virtual void visit(Messages::PhoneReset *msg) = 0;
    virtual void visit(Messages::LPMToNormalCompleted *msg) = 0;
};
}

#endif
