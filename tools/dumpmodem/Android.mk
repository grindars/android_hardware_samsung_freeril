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

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := main.cpp

LOCAL_LDLIBS = -lpthread
LOCAL_SHARED_LIBRARIES = libstlport
LOCAL_C_INCLUDES = external/stlport/stlport bionic $(LOCAL_PATH)/../../libandroidhal $(LOCAL_PATH)/../../libsamsung-ipc
LOCAL_STATIC_LIBRARIES = libSamsungIPC libAndroidHAL

LOCAL_MODULE := dumpmodem
LOCAL_MODULE_TAGS := eng

include $(BUILD_EXECUTABLE)