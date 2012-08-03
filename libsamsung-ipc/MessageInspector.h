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

#ifndef __MESSAGE_INSPECTOR__H__
#define __MESSAGE_INSPECTOR__H__

#include "IMessageVisitor.h"

namespace SamsungIPC {
    class MessageInspector: public IMessageVisitor {
    public:
        virtual void visit(Messages::PowerCompleted *msg);
        virtual void visit(Messages::PhonePowerOff *msg);
        virtual void visit(Messages::PhoneReset *msg);
        virtual void visit(Messages::LPMToNormalCompleted *msg);
    };
}

#endif
