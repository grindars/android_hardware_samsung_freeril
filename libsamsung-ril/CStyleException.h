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

#ifndef __SAMSUNGRIL__CSTYLEXCEPTION__H__
#define __SAMSUNGRIL__CSTYLEXCEPTION__H__

#include <exception>
#include <string>

namespace RIL {
    class CStyleException: public std::exception {
    public:
        CStyleException();
        virtual ~CStyleException() throw();

        virtual const char *what() const throw();

    private:
        std::string m_error;
    };

    inline void throwErrno() { throw CStyleException(); }
}

#endif
