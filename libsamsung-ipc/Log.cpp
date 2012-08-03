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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "Log.h"
#include "LogSink.h"

using namespace SamsungIPC;

static void stdlog_generic(const char *fmt, va_list list, void (*receiver)(const std::string &msg)) {
    char *string = NULL;

    int size = vasprintf(&string, fmt, list);

    receiver(std::string(string, size));
    free(string);
}

void Log::debug(const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);

    stdlog_generic(fmt, list, Log::debug);

    va_end(list);
}

void Log::info(const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);

    stdlog_generic(fmt, list, Log::info);

    va_end(list);
}

void Log::warning(const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);

    stdlog_generic(fmt, list, Log::warning);

    va_end(list);
}
void Log::error(const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);

    stdlog_generic(fmt, list, Log::error);

    va_end(list);
}

void Log::panicErrno(const char *what) {
    panic("%s: %s", what, strerror(errno));
}

void Log::panic(const char *fmt, ...) {
    error("Radio in panic:");

    va_list list;
    va_start(list, fmt);

    stdlog_generic(fmt, list, Log::error);

    va_end(list);

    abort();
}

void Log::deliver(Level level, const std::string &string) {
    for(std::set<LogSink *>::const_iterator it = m_sinkList.begin(); it != m_sinkList.end(); it++) {
        (*it)->print(level, string);
    }
}

std::set<LogSink *> Log::m_sinkList;