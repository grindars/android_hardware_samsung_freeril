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

#include <string.h>

#include "Message.h"
#include "PowerMessages.h"

using namespace SamsungIPC;
using namespace SamsungIPC::Messages;

Message::~Message() {

}

Message *Message::createFromDispatchTable(const MessageFactory table[],
                                          int first, int last, int value,
                                          const Header &header,
                                          const void *data) {

    if(value < first || value > last)
        return NULL;

    MessageFactory factory = table[value - first];
    if(factory)
        return factory(header, data);
    else
        return NULL;
}

Message *Message::create(const Header &header, const void *data) {
    return createFromDispatchTable(m_mainDispatch, MAIN_CMD_FIRST,
                                   MAIN_CMD_LAST, header.mainCommand, header,
                                   data);
}

Message *Message::createPWR(const Header &header, const void *data) {
    return createFromDispatchTable(m_powerDispatch, POWER_CMD_FIRST,
                                   POWER_CMD_LAST, header.subCommand, header,
                                   data);
}

/*
Message *Message::createCALL(const Header &header, const void *data) {
    return createFromDispatchTable(m_callDispatch, CALL_CMD_FIRST,
                                   CALL_CMD_LAST, header, data);
}*/

Message *Message::createCALL(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createSMS(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createSEC(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createDISP(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createNET(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createSND(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createMISC(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createSVC(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createSS(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createGPRS(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createSAT(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createCFG(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createIMEI(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createGPS(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createFACTORY(const Header &header, const void *data) {
    return NULL;
}

Message *Message::createRFS(const Header &header, const void *data) {
    return NULL;
}

const Message::MessageFactory Message::m_mainDispatch[MAIN_CMD_LAST - MAIN_CMD_FIRST + 1] = {
    Message::createPWR,
    Message::createCALL,
    NULL,
    Message::createSMS,
    Message::createSEC,
    NULL,
    Message::createDISP,
    Message::createNET,
    Message::createSND,
    Message::createMISC,
    Message::createSVC,
    Message::createSS,
    Message::createGPRS,
    Message::createSAT,
    Message::createCFG,
    Message::createIMEI,
    Message::createGPS,
    NULL,
    Message::createFACTORY,
    NULL,
    Message::createRFS
};

const Message::MessageFactory Message::m_powerDispatch[POWER_CMD_LAST - POWER_CMD_FIRST + 1] = {
    PowerCompleted::parse,
    PhonePowerOff::parse,
    PhoneReset::parse,
    NULL,
    NULL,
    NULL,
    LPMToNormalCompleted::parse
};
