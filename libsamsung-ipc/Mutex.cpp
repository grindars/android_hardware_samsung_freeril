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

#include <errno.h>

#include "Mutex.h"
#include "CStyleException.h"

using namespace SamsungIPC;

Mutex::Mutex() {
    if(pthread_mutex_init(&m_mutex, NULL) == -1)
        throwErrno();
}

Mutex::~Mutex() {
    if(pthread_mutex_destroy(&m_mutex) == -1)
        throwErrno();
}

void Mutex::lock() {
    if(pthread_mutex_lock(&m_mutex) == -1)
        throwErrno();
}

bool Mutex::tryLock() {
    int ret = pthread_mutex_trylock(&m_mutex);

    if(ret == -1 && errno == EBUSY)
        return false;
    else if(ret == -1)
        throwErrno();

    return true;
}

void Mutex::unlock() {
    if(pthread_mutex_unlock(&m_mutex) == -1)
        throwErrno();
}

