/*
 * Free HAL implementation for Samsung Android-based smartphones.
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
#include <memory>
#include <errno.h>
#include <Log.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <openssl/md5.h>

#include "AndroidFileSystem.h"
#include "NativeFile.h"

#define NVDATA_SIZE (2 * 1024 * 1024)
#define CORE_SIZE   (1 * 1024 * 1024)

using namespace SamsungIPC;
using namespace HAL;

static const unsigned char samsung_magic_words[] = {
    'S', 'a', 'm', 's', 'u', 'n', 'g', '_',
    'A', 'n', 'd', 'r', 'o', 'i', 'd', '_',
    'R', 'I', 'L'
};

AndroidFileSystem::AndroidFileSystem(const std::string &firmware,
                                     const std::string &efs) :
    m_firmware(firmware), m_efs(efs) {

    std::string md5_marker_filename = efs + "/.nv_state";
    m_md5_enabled = access(md5_marker_filename.c_str(), R_OK) == 0;

    if(m_md5_enabled) {
        std::vector<unsigned char> content;
        NativeFile::get(md5_marker_filename, content);

        m_md5_enabled = content.size() != 1 || content[0] != '0';
    }

    m_nvdata = tryLoadNV(efs + "/nv_data.bin", m_md5_enabled, NVDATA_SIZE);

    if(m_nvdata == -1) {
        Log::error("nv_data load failed, trying backup");

        m_nvdata = tryLoadNV(efs + "/.nv_data.bak", m_md5_enabled, NVDATA_SIZE);

        if(m_nvdata != -1) {
            Log::info("recovering nvdata from backup");

            int nvdata = open((efs + "/nv_data.bin").c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);
            if(nvdata == -1)
                Log::panicErrno("open");

            off_t offset = 0;
            ssize_t ret = sendfile(nvdata, m_nvdata, &offset, NVDATA_SIZE);

            if(ret != NVDATA_SIZE)
                Log::panicErrno("sendfile");

            close(m_nvdata);
            m_nvdata = nvdata;

            updateMD5();
        }
    }

    if(m_nvdata == -1) {
        Log::error("nv_data.bak load failed, trying secondary backup");

        m_nvdata = tryLoadNV(efs + "/.nv_core.bak", m_md5_enabled, CORE_SIZE);

        if(m_nvdata != -1) {
            Log::info("recovering nvdata and primary backup from secondary backup");

            /*
             * It's unwise to use backupNVData here to rebuild nv_data.bak,
             * because it rewrites nv_core, and we will be doomed in case of
             * failure.
             */

            unsigned char *padding = new unsigned char[NVDATA_SIZE - CORE_SIZE];
            memset(padding, 0xFF, NVDATA_SIZE - CORE_SIZE);

            int nvdata = open((efs + "/nv_data.bin").c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);
            if(nvdata == -1)
                Log::panicErrno("open");

            int backup = open((efs + "/.nv_data.bak").c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);
            if(backup == -1)
                Log::panicErrno("open");

            off_t offset = 0;
            ssize_t ret = sendfile(nvdata, m_nvdata, &offset, CORE_SIZE);
            if(ret != CORE_SIZE)
                Log::panicErrno("sendfile");

            offset = 0;
            ret = sendfile(backup, m_nvdata, &offset, CORE_SIZE);
            if(ret != CORE_SIZE)
                Log::panicErrno("sendfile");

            ret = write(nvdata, padding, NVDATA_SIZE - CORE_SIZE);
            if(ret != NVDATA_SIZE - CORE_SIZE)
                Log::panicErrno("write");

            ret = write(backup, padding, NVDATA_SIZE - CORE_SIZE);
            if(ret != NVDATA_SIZE - CORE_SIZE)
                Log::panicErrno("write");

            fsync(backup);
            close(m_nvdata);
            m_nvdata = nvdata;
            fsync(m_nvdata);
            close(backup);

            m_nvdata = nvdata;

            unsigned char digest[32];

            if(!calculateFileMD5(m_nvdata, 0, NVDATA_SIZE, digest))
                Log::panicErrno("calculateFileMD5");

            NativeFile::put(efs + "/.nv_data.bak.md5", std::vector<unsigned char>(digest, digest + sizeof(digest)), true);
            NativeFile::put(efs + "/.nv_data.md5", std::vector<unsigned char>(digest, digest + sizeof(digest)), true);
        }
    }

    if(m_nvdata == -1) {
        Log::error(
            "=========================== WARNING ===========================\n"
            "NV Data is damaged and all attempts to recover it have failed.\n"
            "Proceeding with default (blank) NV data.\n"
            "\n"
            "In layman's terms, you are screwed.\n"
            "\n"
            "You have made a backup of /efs before, aren't you?\n"
            "===============================================================\n"
        );

        m_nvdata = open((efs + "/nv_data.bin").c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);
        if(m_nvdata == -1)
            Log::panicErrno("open");

        std::vector<unsigned char> data;
        getFirmware(SamsungIPC::IFileSystem::DefaultNVData, data);

        ssize_t ret = write(m_nvdata, &data[0], data.size());

        if(ret != NVDATA_SIZE)
            Log::panicErrno("write");

        updateMD5();
        backupNVData(true);
    }
}

void AndroidFileSystem::updateMD5() {
    unsigned char digest[32];
    if(!calculateFileMD5(m_nvdata, 0, NVDATA_SIZE, digest))
        Log::panicErrno("calculateFileMD5");

    int md5 = open((m_efs + "/nv_data.bin.md5").c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0600);
    if(md5 == -1)
        Log::panicErrno("open");

    ssize_t ret = write(md5, digest, sizeof(digest));
    if(ret != sizeof(digest))
        Log::panicErrno("write");

    close(md5);

    if(!m_md5_enabled) {
        int marker = open((m_efs + "/.nv_state").c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0600);

        if(marker == -1)
            Log::panicErrno("open");

        char value = '1';

        ret = write(marker, &value, sizeof(value));

        if(ret != sizeof(value))
            Log::panicErrno("write");

        fsync(marker);
        close(marker);

        m_md5_enabled = true;
    }
}

int AndroidFileSystem::tryLoadNV(const std::string filename, bool md5_enabled, size_t expected_size) {
    int fd = open(filename.c_str(), O_RDWR);
    if(fd == -1) {
        Log::error("%s open failed: %s", filename.c_str(), strerror(errno));

        return -1;
    }

    struct stat statbuf;
    if(fstat(fd, &statbuf) == -1) {
        close(fd);

        return -1;
    }

    if(statbuf.st_size != expected_size) {
        Log::error("%s have unexpected size: %u bytes, expected %u bytes", filename.c_str(), statbuf.st_size, expected_size);

        close(fd);

        return -1;
    }

    if(md5_enabled) {
        std::string digest = filename + ".md5";

        if(access(digest.c_str(), R_OK) != 0) {
            Log::error("%s not found - assuming invalid", digest.c_str());

            close(fd);

            return -1;
        }

        std::vector<unsigned char> md5;
        unsigned char calculated_md5[32];

        NativeFile::get(digest.c_str(), md5);

        if(!calculateFileMD5(fd, 0, expected_size, calculated_md5)) {
            close(fd);

            return -1;
        }

        if(md5.size() != sizeof(calculated_md5) || memcmp(&md5[0], calculated_md5, sizeof(calculated_md5)) != 0) {
            Log::error("%s is corrupted", filename.c_str());

            close(fd);

            return -1;
        }
    }

    return fd;
}

void AndroidFileSystem::calculateMD5(const void *data, size_t size, unsigned char digest[32]) {
    MD5_CTX ctx;

    unsigned char binary[16];

    MD5_Init(&ctx);
    MD5_Update(&ctx, data, size);
    MD5_Update(&ctx, samsung_magic_words, sizeof(samsung_magic_words));
    MD5_Final(binary, &ctx);

    binaryToHex(binary, sizeof(binary), digest);
}

bool AndroidFileSystem::calculateFileMD5(int fd, off_t offset, size_t size, unsigned char digest[32]) {
    MD5_CTX ctx;

    unsigned char binary[16];

    MD5_Init(&ctx);

    char *buf = new char[16384];

    for(size_t i = offset; i < offset + size; i += 16384) {
        ssize_t bytes = pread(fd, buf, 16384, i);

        if(bytes == -1) {
            delete[] buf;

            return false;
        }

        MD5_Update(&ctx, buf, bytes);
    }

    delete[] buf;

    MD5_Update(&ctx, samsung_magic_words, sizeof(samsung_magic_words));
    MD5_Final(binary, &ctx);

    binaryToHex(binary, sizeof(binary), digest);

    return true;
}

void AndroidFileSystem::binaryToHex(const unsigned char *data, size_t size, unsigned char *out) {
    char buf[3];

    for(unsigned char *ptr = out; ptr < out + size * 2; ptr += 2) {
        snprintf(buf, sizeof(buf), "%02hhx", *data++);

        ptr[0] = buf[0];
        ptr[1] = buf[1];
    }
}

bool AndroidFileSystem::getFirmware(SamsungIPC::IFileSystem::FirmwareType type, std::vector<unsigned char> &data) {
    unsigned int offset, length;

    switch(type) {
    case SamsungIPC::IFileSystem::PSI:
        offset = 0;
        length = 61440;

        break;

    case SamsungIPC::IFileSystem::EBL:
        offset = 61440;
        length = 102400;

        break;

    case SamsungIPC::IFileSystem::SecureImage:
        offset = 10483712;
        length = 2048;

        break;

    case SamsungIPC::IFileSystem::Firmware:
        offset = 163840;
        length = 10321920;

        break;

    case SamsungIPC::IFileSystem::DefaultNVData:
        offset = 10485760;
        length = NVDATA_SIZE;

        break;

    default:
        Log::panic("Invalid firmware type");

        break;
    }

    HAL::NativeFile file = HAL::NativeFile::open(m_firmware, O_RDONLY);

    data.resize(length);

    ssize_t bytes = file.pread(&data[0], length, offset);
    if((unsigned int) bytes != length) {
        Log::panic("Short read");
    }

    return true;
}

int AndroidFileSystem::readNVData(off_t offset, std::vector<unsigned char> &data) {
    if(offset + data.size() > NVDATA_SIZE)
        return -ERANGE;

    int ret = pread(m_nvdata, &data[0], data.size(), offset);

    if(ret == -1)
        return -errno;
    else
        return 0;
}

int AndroidFileSystem::writeNVData(off_t offset, const std::vector<unsigned char> &data) {
    if(offset + data.size() > NVDATA_SIZE)
        return -ERANGE;

    int ret = pwrite(m_nvdata, &data[0], data.size(), offset);

    if(ret == -1)
        return -errno;
    else {
        updateMD5();

        return 0;
    }
}

void AndroidFileSystem::backupNVData(bool full) {
    if(!m_md5_enabled)
        updateMD5();

    std::vector<unsigned char> digest;

    NativeFile::get(m_efs + "/nv_data.bin.md5", digest);

    off_t offset = 0;
    int backup = open((m_efs + "/.nv_data.bak").c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if(backup == -1)
        Log::panicErrno("open");

    ssize_t ret = sendfile(backup, m_nvdata, &offset, NVDATA_SIZE);

    if(ret != NVDATA_SIZE)
        Log::panicErrno("sendfile");

    fsync(backup);
    close(backup);
    NativeFile::put(m_efs + "/.nv_data.bak.md5", digest, true);

    if(full) {
        offset = 0;

        backup = open((m_efs + "/.nv_core.bak").c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);
        if(backup == -1)
            Log::panicErrno("open");

        ret = sendfile(backup, m_nvdata, &offset, CORE_SIZE);

        if(ret != CORE_SIZE)
            Log::panicErrno("sendfile");

        unsigned char core_digest[32];
        if(!calculateFileMD5(backup, 0, CORE_SIZE, core_digest))
            Log::panicErrno("calculateFileMD5");

        fsync(backup);
        close(backup);
        NativeFile::put(m_efs + "/.nv_core.bak.md5", std::vector<unsigned char>(core_digest, core_digest + sizeof(core_digest)));
    }
}

int AndroidFileSystem::readFile(int fd, std::vector<unsigned char> &data) {
    if(!isSafeDescriptor(fd))
        return -EBADF;

    return translateReturn(read(fd, &data[0], data.size()));
}

int AndroidFileSystem::writeFile(int fd, const std::vector<unsigned char> &data) {
    if(!isSafeDescriptor(fd))
        return -EBADF;

    return translateReturn(write(fd, &data[0], data.size()));
}

int AndroidFileSystem::lseekFile(int fd, int offset, int whence) {
    if(!isSafeDescriptor(fd))
        return -EBADF;

    return translateReturn(lseek(fd, offset, whence));
}

int AndroidFileSystem::closeFile(int fd) {
    std::tr1::unordered_set<int>::iterator it = m_rfsDescriptors.find(fd);

    if(it == m_rfsDescriptors.end())
        return -EBADF;

    int ret = close(fd);

    if(ret == 0)
        m_rfsDescriptors.erase(it);

    return translateReturn(ret);
}

int AndroidFileSystem::renameFile(const std::string &oldName, const std::string &newName) {
    std::string oldExpanded, newExpanded;

    int ret = expandAndValidateFilename(oldName, oldExpanded);

    if(ret != 0)
        return ret;

    ret = expandAndValidateFilename(newName, newExpanded);

    if(ret != 0)
        return ret;

    ret = makePath(newExpanded);

    if(ret != 0)
        return ret;

    return translateReturn(rename(oldExpanded.c_str(), newExpanded.c_str()));
}

int AndroidFileSystem::getFileInfoByHandle(int fd, struct stat *statbuf) {
    if(!isSafeDescriptor(fd))
        return -EBADF;

    return translateReturn(fstat(fd, statbuf));
}

int AndroidFileSystem::deleteFile(const std::string &filename) {
    std::string expanded;

    int ret = expandAndValidateFilename(filename, expanded);

    if(ret != 0)
        return ret;

    return translateReturn(unlink(expanded.c_str()));
}

int AndroidFileSystem::makeDirectory(const std::string &filename) {
    std::string expanded;

    int ret = expandAndValidateFilename(filename, expanded);

    if(ret != 0)
        return ret;

    ret = makePath(expanded);

    if(ret != 0)
        return ret;

    return translateReturn(mkdir(expanded.c_str(), 0700));
}

int AndroidFileSystem::deleteDirectory(const std::string &filename) {
    std::string expanded;

    int ret = expandAndValidateFilename(filename, expanded);

    if(ret != 0)
        return ret;

    ret = makePath(expanded);

    if(ret != 0)
        return ret;

    return translateReturn(rmdir(expanded.c_str()));
}

int AndroidFileSystem::openDirectory(const std::string &filename) {
    std::string expanded;

    int ret = expandAndValidateFilename(filename, expanded);

    if(ret != 0)
        return ret;

    ret = makePath(expanded);

    if(ret != 0)
        return ret;

    DIR *dir = opendir(expanded.c_str());
    if(dir == NULL)
        return -errno;

    m_rfsDirectories.insert(dir);

    return (int) dir;
}

int AndroidFileSystem::readDirectory(int fd, std::string &filename) {
    DIR *dir = (DIR *) fd;

    if(m_rfsDirectories.find(dir) == m_rfsDirectories.end())
        return -EBADF;

    struct dirent buf, *entry;

    int ret = readdir_r(dir, &buf, &entry);

    if(ret != 0)
        return -ret;

    if(entry == NULL)
        return 0;

    filename.assign(entry->d_name);
    return 1;
}

int AndroidFileSystem::closeDirectory(int fd) {
    DIR *dir = (DIR *) fd;

    std::tr1::unordered_set<DIR *>::iterator it = m_rfsDirectories.find(dir);
    if(it == m_rfsDirectories.end())
        return -EBADF;

    int ret = closedir(*it);
    if(ret == 0)
        m_rfsDirectories.erase(it);

    return translateReturn(ret);
}

int AndroidFileSystem::openFile(const std::string &filename, int flags) {
    std::string expanded;

    int ret = expandAndValidateFilename(filename, expanded);

    if(ret != 0)
        return ret;

    ret = makePath(expanded);

    if(ret != 0)
        return ret;

    int fd = open(expanded.c_str(), flags, 0600);

    if(fd >= 0)
        m_rfsDescriptors.insert(fd);

    return translateReturn(fd);
}

int AndroidFileSystem::ftruncateFile(int fd, unsigned int size) {
    if(!isSafeDescriptor(fd))
        return -EBADF;

    return translateReturn(ftruncate(fd, size));
}

int AndroidFileSystem::expandAndValidateFilename(const std::string &filename, std::string &expanded) {

    if(filename.length() > 0 && filename[0] == '/')
        expanded.assign(filename);
    else
        expanded.assign(m_efs + "/" + filename);

    for(size_t offset = 0, next_offset; offset != std::string::npos; offset = next_offset) {
        next_offset = expanded.find('/', offset + 1);

        if(offset == 0 && expanded.compare(0, next_offset, m_efs) != 0)
            return -EACCES;

        std::string component = expanded.substr(offset + 1, next_offset == std::string::npos ? std::string::npos : next_offset - offset);
        if(component == "/..")
            return -EACCES;
    }

    return 0;
}

int AndroidFileSystem::makePath(const std::string &expanded) {
    for(size_t offset = 0; offset != std::string::npos; offset = expanded.find('/', offset + 1)) {
        if(offset != 0) {
            std::string directory = expanded.substr(0, offset);

            int ret = mkdir(directory.c_str(), 0700);

            if(ret == -1 && errno != EEXIST)
                return -errno;
        }
    }

    return 0;
}

bool AndroidFileSystem::isSafeDescriptor(int fd) {
    return m_rfsDescriptors.find(fd) != m_rfsDescriptors.end();
}

int AndroidFileSystem::translateReturn(int ret) {
    if(ret == -1)
        return -errno;
    else
        return ret;
}

