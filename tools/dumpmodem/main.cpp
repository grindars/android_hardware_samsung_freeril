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

#include <fstream>

#include <SamsungModem.h>
#include <AndroidHAL.h>
#include <CStyleException.h>
#include <IProgressCallback.h>

#include <stdio.h>
#include <sys/ioctl.h>

#define DEFAULT_DUMP "dump.bin"

class ConsoleProgressCallback: public SamsungIPC::IProgressCallback {
public:
    ConsoleProgressCallback() : m_prevProgress(-1) {
        struct winsize size;

        if(isatty(STDOUT_FILENO)) {
            if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1) {
                m_max = 0;
                return;
            }


        } else {
            m_max = 0;

            return;
        }

        if(size.ws_col <= 6)
            m_max = 80 - 6;
        else
            m_max = size.ws_col - 6;
    }

    ~ConsoleProgressCallback() {
        if(m_max != 0) {
            putchar('\r');

            for(unsigned int i = 0; i < m_max + 6; i++)
                putchar(' ');

            putchar('\r');

            fflush(stdout);
        }
    }

    virtual void progress(size_t value, size_t max) {
        unsigned int progress, percent;
        if(m_max != 0) {
            progress = value * m_max / max;
            percent = value * 100 / max;

            if(progress != m_prevProgress) {
                fputs("\r|", stdout);

                for(unsigned int i = 0; i < progress; i++) {
                    putchar('=');
                }

                for(unsigned int i = progress; i < m_max; i++)
                    putchar(' ');

                printf("|%3d%%", percent);

                fflush(stdout);

                m_prevProgress = progress;
            }
        }
    }

private:
    unsigned int m_max, m_prevProgress;
};

static void usage(const char *app) {
    printf("Usage: %s [OPTIONS]\n"
           "Baseband uploading tool.\n"
           "\n"
           "Options:\n"
           "  -o <FILE> Write to specified file instead of " DEFAULT_DUMP ".\n",
           app);
}

int main(int argc, char *argv[]) {
    std::string output = DEFAULT_DUMP;
    int ch;

    while((ch = getopt(argc, argv, "o:")) != -1) {
        switch(ch) {
            case 'o':
                output.assign(optarg);

                break;

            case '?':
            case ':':
                usage(argv[0]);

                return 1;
        }
    }

    HAL::AndroidHAL hal;

    SamsungIPC::SamsungModem modem(&hal);

    try {
        std::ofstream stream;
        stream.open(output.c_str(), std::ios_base::binary | std::ios_base::out |
                                    std::ios_base::trunc);
        if(stream.fail())
            HAL::throwErrno();

        ConsoleProgressCallback callback;
        modem.dump(stream, &callback);

        stream.close();
    } catch(std::exception &e) {
        fprintf(stderr, "Dump failed: %s\n", e.what());
        unlink(output.c_str());

        return 1;
    }

    printf("Modem dumped to %s\n", output.c_str());

    return 0;
}
