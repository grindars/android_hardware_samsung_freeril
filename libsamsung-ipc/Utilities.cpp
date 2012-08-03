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
#include <ctype.h>

#include <algorithm>

#include "Log.h"
#include "Utilities.h"

void SamsungIPC::dump(const void *data, size_t size) {
    const unsigned char *cdata = (const unsigned char *) data;

    char line[90];

    for(size_t row = 0; row < size; row += 16) {
        int pos = 0;

        pos += sprintf(&line[pos], "%08X  ", row);

        size_t row_size = std::min<size_t>(size - row, 16);

        for(size_t row_index = 0; row_index < row_size; row_index++) {
            if(row_index == 8)
                line[pos++] = ' ';

            pos += sprintf(&line[pos], "%02hhX ", cdata[row + row_index]);
        }

        for(size_t row_index = row_size; row_index < 16; row_index++) {
            if(row_index == 8)
                line[pos++] = ' ';

            line[pos++] = ' ';
            line[pos++] = ' ';
            line[pos++] = ' ';
        }

        line[pos++] = ' ';
        line[pos++] = '|';

        for(size_t row_index = 0; row_index < row_size; row_index++) {
            char ch = (char) cdata[row + row_index];

            line[pos++] = isprint(ch) ? ch : '.';
        }

        line[pos++] = '|';
        line[pos++] = 0;

        Log::debug("%s", line);
    }
}
