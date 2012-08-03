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

#include "UnsolicitedHandler.h"

using namespace SamsungIPC;

void UnsolicitedHandler::visit(Messages::PowerCompleted *msg) {
    printf("PowerCompleted: %p\n", msg);
}

void UnsolicitedHandler::visit(Messages::PhonePowerOff *msg) {
    printf("PhonePowerOff: %p\n", msg);
}

void UnsolicitedHandler::visit(Messages::PhoneReset *msg) {
    printf("PhoneReset: %p\n", msg);
}

void UnsolicitedHandler::visit(Messages::LPMToNormalCompleted *msg) {
    printf("LPMToNormalCompleted: %p\n", msg);
}
