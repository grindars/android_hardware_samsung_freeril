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

#include "Semaphore.h"
#include "Log.h"

using namespace SamsungIPC;

Semaphore::Semaphore() {
    if(sem_init(&m_sem, 0, 0) == -1) {
        Log::panicErrno("sem_init");
    }
}

Semaphore::~Semaphore() {
    if(sem_destroy(&m_sem) == -1)
        Log::panicErrno("sem_destroy");
}

void Semaphore::give() {
    if(sem_post(&m_sem) == -1)
        Log::panicErrno("sem_post");
}

void Semaphore::take() {
    if(sem_wait(&m_sem) == -1)
        Log::panicErrno("sem_wait");
}

