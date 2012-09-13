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

#include "MessageCache.h"

MessageCache::MessageCache() {

}

MessageCache::~MessageCache() {
    flush();
}

void MessageCache::put(const std::string &key, SamsungIPC::Message *message) {
    invalidate(key);

    m_messages.insert(std::pair<std::string, SamsungIPC::Message *>(key, message));
}

void MessageCache::invalidate(const std::string &key) {
    std::tr1::unordered_map<std::string, SamsungIPC::Message *>::iterator it = m_messages.find(key);

    if(it != m_messages.end()) {
        delete (*it).second;

        m_messages.erase(it);
    }
}

void MessageCache::flush() {
    for(std::tr1::unordered_map<std::string, SamsungIPC::Message *>::iterator it = m_messages.begin(); it != m_messages.end(); it++) {
        delete (*it).second;
    }

    m_messages.clear();
}

template<> SamsungIPC::Message *MessageCache::get<SamsungIPC::Message>(const std::string &key) {
    std::tr1::unordered_map<std::string, SamsungIPC::Message *>::iterator it = m_messages.find(key);

    if(it == m_messages.end()) {
        return NULL;
    } else {
        return (*it).second;
    }
}
