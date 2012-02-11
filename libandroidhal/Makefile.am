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

lib_LIBRARIES = libandroidhal.a

libandroidhal_a_SOURCES = SysfsEHCISwitcher.h SysfsEHCISwitcher.cpp \
	NativeFile.h NativeFile.cpp CStyleException.h CStyleException.cpp \
	AndroidHAL.h AndroidHAL.cpp SysfsModemControl.h SysfsModemControl.cpp \
	SysfsControlledDevice.h SysfsControlledDevice.cpp PHONETIPCTransport.h \
	PHONETIPCTransport.cpp PHONETIPCSocket.h PHONETIPCSocket.cpp \
	AndroidFileSystem.cpp AndroidFileSystem.h

AM_CPPFLAGS = -I../libsamsung-ipc
AM_CXXFLAGS = -Wall -W -O2
