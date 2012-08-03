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

#include <SamsungModem.h>
#include <AndroidHAL.h>
#include <stdio.h>
#include <poll.h>

#define MAX_BOOT_TRIES  4

int main(void) {
    HAL::AndroidHAL hal;

    SamsungIPC::SamsungModem modem(&hal);

    puts("Booting modem\n");

    int bootTry = 0;
    do {
        try {
            modem.boot();

            break;
        } catch(std::exception &e) {
            fprintf(stderr, "Boot failed, try %d/%d: %s\n",
                ++bootTry, MAX_BOOT_TRIES, e.what());

            if(bootTry == MAX_BOOT_TRIES)
                return 1;
        }
    } while(1);

    puts("Initializing modem");

    try {
        modem.initialize();
    } catch(std::exception &e) {
        fprintf(stderr, "Modem initialization failed: %s\n", e.what());

        return 1;
    }

    puts("Modem operational.");

    poll(NULL, 0, -1);

    return 0;
}
