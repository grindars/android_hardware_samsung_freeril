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

#ifndef __IPC_WORKER_THREAD__H__
#define __IPC_WORKER_THREAD__H__

#include <list>

#include "WorkerThread.h"

namespace SamsungIPC {
    class SocketHandler;

    class IPCWorkerThread: public WorkerThread {
    public:
        IPCWorkerThread();

        void addHandler(SocketHandler *handler);
        void removeHandler(SocketHandler *handler);

    protected:
        virtual int run();

    private:
        void dispatchEvents();

        std::list<SocketHandler *> m_handlers;
    };
}

#endif
