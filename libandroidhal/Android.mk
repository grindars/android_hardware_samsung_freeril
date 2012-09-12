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

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    NativeFile.cpp AndroidHAL.cpp SysfsControlledDevice.cpp USBIPCTransport.cpp \
    USBIPCSocket.cpp AndroidFileSystem.cpp

LOCAL_MODULE = libAndroidHAL
LOCAL_LDLIBS = -lpthread
LOCAL_SHARED_LIBRARIES = libstlport
LOCAL_C_INCLUDES = external/stlport/stlport external/openssl/include bionic $(LOCAL_PATH)/../libsamsung-ipc
LOCAL_CFLAGS = -fvisibility=hidden

include $(BUILD_STATIC_LIBRARY)