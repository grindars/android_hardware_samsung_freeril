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

#include <stdio.h>
#include "PowerMessages.h"
#include "MessageInspector.h"

using namespace SamsungIPC;

void MessageInspector::visit(Messages::PowerCompleted *msg) {
    (void) msg;

    printf("PowerCompleted\n"
           "\n");
}

void MessageInspector::visit(Messages::PhonePowerOff *msg) {
    printf("PhonePowerOff\n"
           " - reason: %d\n"
           "\n", msg->reason());
}

void MessageInspector::visit(Messages::PhoneReset *msg) {
    (void) msg;

    printf("PhoneReset\n"
           "\n");
}

void MessageInspector::visit(Messages::LPMToNormalCompleted *msg) {
    (void) msg;

    printf("LPMToNormalCompleted\n"
           "\n");
}
