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

LOCAL_MODULE := freeril-i9100-database.db
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := VENDOR_ETC
LOCAL_DATABASE_PATH := $(call local-intermediates-dir)
LOCAL_OPTIONAL_PROPRIETARY_RIL := vendor/samsung/galaxys2/proprietary/libsec-ril.so

include $(BUILD_SYSTEM)/base_rules.mk

$(LOCAL_DATABASE_PATH)/operators.sql: PRIVATE_RIL := $(LOCAL_OPTIONAL_PROPRIETARY_RIL)
$(LOCAL_DATABASE_PATH)/operators.sql: PRIVATE_SHIPPED_DB := $(LOCAL_PATH)/operators.shipped.sql
$(LOCAL_DATABASE_PATH)/operators.sql: $(HOST_OUT_EXECUTABLES)/secril-database-extractor
	@mkdir -p $(dir $@)
	@if [ -f $(PRIVATE_RIL) ]; then \
		echo "Extracting: $@" && \
		secril-database-extractor -i $(PRIVATE_RIL) -o $@; \
	else \
		if [ ! -f $(PRIVATE_SHIPPED_DB) ]; then \
			echo "Proprietary RIL not available and no database shipped." 1>&2; \
			exit 1; \
		fi; \
		echo "Shipped: $@"; \
		cp $(PRIVATE_SHIPPED_DB) $@; \
	fi


$(LOCAL_BUILT_MODULE): $(LOCAL_DATABASE_PATH)/operators.sql
	@echo "Generating: $@"
	@rm -f $@
	@sqlite3 $@ < $<