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

#include <fcntl.h>

#include <AndroidFileSystem.h>
#include <NativeFile.h>

#include <stdio.h>

static void usage(const char *app) {
    printf("Usage: %s [OPTIONS]\n"
           "Baseband firmware extraction tool.\n"
           "\n"
           "Options:\n"
           "  -i <FILE>      Use specified input file instead of eMMC partition\n"
           "  -o <DIRECTORY> Write to specified directory instead of working directory.\n",
           app);
}

static void extractToFile(SamsungIPC::IFileSystem *fs,
                          const std::string &filename,
                          SamsungIPC::IFileSystem::FirmwareType type) {
    std::string image = fs->getFirmware(type);

    HAL::NativeFile file = HAL::NativeFile::open(filename, O_WRONLY | O_TRUNC |
                                                 O_CREAT);
    file.write(image.data(), image.length());
}

int main(int argc, char *argv[]) {
    std::string input = "/dev/block/mmcblk0p8", outdir = ".";
    int ch;

    while((ch = getopt(argc, argv, "i:o:")) != -1) {
        switch(ch) {
        case 'i':
            input.assign(optarg);

            break;

        case 'o':
            outdir.assign(optarg);

            break;

        case '?':
        case ':':
            usage(argv[0]);

            return 1;
        }
    }

    try {
        HAL::AndroidFileSystem fs(input, "");

        extractToFile(&fs, outdir + "/psi.bin", SamsungIPC::IFileSystem::PSI);
        extractToFile(&fs, outdir + "/ebl.bin", SamsungIPC::IFileSystem::EBL);
        extractToFile(&fs, outdir + "/secure_image.bin", SamsungIPC::IFileSystem::SecureImage);
        extractToFile(&fs, outdir + "/firmware.bin", SamsungIPC::IFileSystem::Firmware);
        extractToFile(&fs, outdir + "/default_nvdata.bin", SamsungIPC::IFileSystem::DefaultNVData);

        return 0;
    } catch(std::exception &e) {
        fprintf(stderr, "Firmware extraction failed: %s\n", e.what());

        return 1;
    }
}
