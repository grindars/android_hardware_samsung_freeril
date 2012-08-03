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

#ifndef __LOG__H__
#define __LOG__H__

#include <string>
#include <set>

namespace SamsungIPC {
    class LogSink;

    class Log {
    public:
        enum Level {
            Debug,
            Info,
            Warning,
            Error
        };

        static void panicErrno(const char *what) __attribute__((noreturn));
        static void panic(const char *fmt, ...) __attribute__((noreturn));

        static void debug(const char *fmt, ...);
        static void info(const char *fmt, ...);
        static void warning(const char *fmt, ...);
        static void error(const char *fmt, ...);

        static inline void debug(const std::string &string) { deliver(Debug, string); }
        static inline void info(const std::string &string) { deliver(Info, string); }
        static inline void warning(const std::string &string) { deliver(Warning, string); }
        static inline void error(const std::string &string) { deliver(Error, string); }


    private:
        friend class LogSink;

        static inline void registerSink(LogSink *sink) {
            m_sinkList.insert(sink);
        }

        static inline void unregisterSink(LogSink *sink) {
            m_sinkList.erase(m_sinkList.find(sink));
        }

    private:
        static void deliver(Level level, const std::string &string);

        static std::set<LogSink *> m_sinkList;
    };
}

#endif
