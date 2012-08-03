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

#include "WorkerThread.h"
#include "CStyleException.h"
#include "Exceptions.h"

using namespace SamsungIPC;

WorkerThread::WorkerThread() : m_thread(0) {

}

WorkerThread::~WorkerThread() {
    if(m_thread != 0) {
        if(pthread_tryjoin_np(m_thread, NULL) == -1 && errno == EBUSY)
            throw InternalErrorException("WorkerThread destroyed while thread is still running");
    }
}

int WorkerThread::wait() {
    if(m_thread == 0)
        throw InternalErrorException("Thread is not yet started");

    void *ret;

    if(pthread_join(m_thread, &ret) == -1)
        throwErrno();

    m_thread = 0;

    return (int) ret;
}

void WorkerThread::start() {
    if(m_thread != 0)
        throw InternalErrorException("Thread is already started");

    if(pthread_create(&m_thread, NULL, WorkerThread::threadRoutine, this) == -1) {
        m_thread = 0;
        throwErrno();
    }
}

void WorkerThread::terminate() {
    if(m_thread == 0)
        throw InternalErrorException("Thread is not yet started");

    if(pthread_cancel(m_thread) == -1)
        throwErrno();
}

void *WorkerThread::threadRoutine(void *arg) {
    WorkerThread *thread = static_cast<WorkerThread *>(arg);

    return (void *) thread->run();
}
