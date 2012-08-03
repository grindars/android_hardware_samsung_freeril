#
# Free RIL implementation for Samsung Android-based smartphones.
# Copyright (C) 2012  Sergey Gridasov <grindars@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

lib_LIBRARIES = libsamsung-ipc.a

libsamsung_ipc_a_SOURCES = SamsungIPCHAL.h SamsungModem.h SamsungModem.cpp \
	IIPCTransport.h Exceptions.h Exceptions.cpp IIPCSocket.h Utilities.cpp \
	Utilities.h IProgressCallback.h WorkerThread.h WorkerThread.cpp \
	IPCWorkerThread.h IPCWorkerThread.cpp CStyleException.h \
	CStyleException.cpp SocketHandler.h SocketHandler.cpp Mutex.h \
	Mutex.cpp IPCSocketHandler.h IPCSocketHandler.cpp RFSSocketHandler.h \
	RFSSocketHandler.cpp Message.h Message.cpp PowerMessages.h \
	PowerMessages.cpp IMessageVisitor.h UnsolicitedHandler.h \
	UnsolicitedHandler.cpp

AM_CXXFLAGS = -Wall -W -O2

AM_CPPFLAGS = -DMESSAGE_INSPECTION
libsamsung_ipc_a_SOURCES += MessageInspector.h MessageInspector.cpp
