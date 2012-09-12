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
    IPCSocketHandler.cpp Message.cpp Mutex.cpp RFSSocketHandler.cpp \
    SocketHandler.cpp Utilities.cpp IPCWorkerThread.cpp SamsungModem.cpp \
    WorkerThread.cpp Log.cpp LogSink.cpp StdoutLogSink.cpp Semaphore.cpp \
    DataStream.cpp

LOCAL_MODULE = libSamsungIPC
LOCAL_LDLIBS = -lpthread
LOCAL_SHARED_LIBRARIES = libstlport
LOCAL_C_INCLUDES = external/stlport/stlport bionic
LOCAL_CFLAGS = -fvisibility=hidden

LOCAL_MODULE_CLASS := STATIC_LIBRARIES
COMPILED_PROTOCOL_PATH := $(call local-intermediates-dir)

GEN := $(addprefix $(COMPILED_PROTOCOL_PATH)/,IUnsolicitedReceiver.h MessageFactory.cpp MessageFactory.h \
                       Messages.cpp Messages.h MessageTypes.h)

$(GEN): PRIVATE_PATH := $(LOCAL_PATH)
$(GEN): PRIVATE_CUSTOM_TOOL := $(PRIVATE_PATH)/compiler/compiler.rb $(COMPILED_PROTOCOL_PATH) $(PRIVATE_PATH)/protocol
$(GEN): $(LOCAL_PATH)/protocol/*.rb $(LOCAL_PATH)/compiler/*.rb $(LOCAL_PATH)/compiler/templates/*.erb
	$(transform-generated-source)

LOCAL_GENERATED_SOURCES = $(GEN)

ifeq ($(TARGET_BUILD_VARIANT),eng)
LOCAL_CFLAGS += -DIPC_DEBUG
endif

include $(BUILD_STATIC_LIBRARY)