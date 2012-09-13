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

#ifndef __MESSAGE_CACHE__H__
#define __MESSAGE_CACHE__H__

#include "string.h"
#include <Message.h>
#include <unordered_map>

class MessageCache {
public:
    MessageCache();
    ~MessageCache();

    template<class T> T *get(const std::string &key) {
        return message_cast<T>(get<SamsungIPC::Message>(key));
    }

    void put(const std::string &key, SamsungIPC::Message *message);
    void invalidate(const std::string &key);
    void flush();

private:
    std::tr1::unordered_map<std::string, SamsungIPC::Message *> m_messages;
};


template<> SamsungIPC::Message *MessageCache::get<SamsungIPC::Message>(const std::string &key);

#endif
