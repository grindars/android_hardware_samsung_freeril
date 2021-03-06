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
#include "Log.h"

using namespace SamsungIPC;

Mutex::Mutex() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    if(pthread_mutex_init(&m_mutex, &attr) == -1)
        Log::panicErrno("pthread_mutex_init");

    pthread_mutexattr_destroy(&attr);
}

Mutex::~Mutex() {
    if(pthread_mutex_destroy(&m_mutex) == -1)
        Log::panicErrno("pthread_mutex_destroy");
}

void Mutex::lock() {
    if(pthread_mutex_lock(&m_mutex) == -1)
        Log::panicErrno("pthread_mutex_lock");
}

bool Mutex::tryLock() {
    int ret = pthread_mutex_trylock(&m_mutex);

    if(ret == -1 && errno == EBUSY)
        return false;
    else if(ret == -1)
        Log::panicErrno("pthread_mutex_trylock");

    return true;
}

void Mutex::unlock() {
    if(pthread_mutex_unlock(&m_mutex) == -1)
        Log::panicErrno("pthread_mutex_unlock");
}

