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

#ifndef __SAMSUNGIPC__EXCEPTIONS__H__
#define __SAMSUNGIPC__EXCEPTIONS__H__

#include <string>

namespace SamsungIPC {
    class BaseStringException: public std::exception {
    protected:
        BaseStringException(const std::string &msg);

    public:
        virtual ~BaseStringException() throw();

        virtual const char *what() const throw();

    private:
        std::string m_msg;
    };

    class TimeoutException: public BaseStringException {
    public:
        TimeoutException(const std::string &msg);
    };

    class CommunicationErrorException: public BaseStringException {
    public:
        CommunicationErrorException(const std::string &msg);
    };

    class InternalErrorException: public BaseStringException {
    public:
        InternalErrorException(const std::string &msg);
    };
}

#endif
