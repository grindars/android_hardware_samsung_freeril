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

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "RFSSocketHandler.h"
#include "Log.h"
#include "MessageFactory.h"
#include "DataStream.h"
#include "Messages.h"
#include "Utilities.h"
#include "IFileSystem.h"

#if defined(TRACE_RFS)
    #define RFS_TRACE(...) Log::debug("RFS: " __VA_ARGS__)
#else
    #define RFS_TRACE(...)
#endif

using namespace SamsungIPC;

template<class T> static void buildReply(T *reply, int ret) {
    if(ret < 0) {
        reply->setRet(-1);
        reply->setErrno(-ret);
    } else {
        reply->setRet(ret);
        reply->setErrno(0);
    }
}

template<class T> static void buildFileInfo(T *reply, const struct stat &statbuf) {
    reply->setType(S_ISDIR(statbuf.st_mode) ? 1 : 2);
    reply->setSize(statbuf.st_size);

    struct tm broken;
    time_t stamp = statbuf.st_ctime;
    localtime_r(&stamp, &broken);
    reply->setCyear(broken.tm_year - 100);
    reply->setCmonth(broken.tm_mon + 1);
    reply->setCday(broken.tm_mday);
    reply->setChour(broken.tm_hour);
    reply->setCminute(broken.tm_min);
    reply->setCsecond(broken.tm_sec);

    stamp = statbuf.st_mtime;
    localtime_r(&stamp, &broken);
    reply->setMyear(broken.tm_year - 100);
    reply->setMmonth(broken.tm_mon + 1);
    reply->setMday(broken.tm_mday);
    reply->setMhour(broken.tm_hour);
    reply->setMminute(broken.tm_min);
    reply->setMsecond(broken.tm_sec);
}

RFSSocketHandler::RFSSocketHandler(IIPCSocket *socket, IFileSystem *fs) : SocketHandler(socket), m_fs(fs) {

}

void RFSSocketHandler::handleMessage(const Message::RFSHeader &header,
                                     const void *data) {

    if(header.type < FirstType || header.type > LastType || m_handlers[header.type - FirstType] == 0) {
        Log::error("No handler for RFS message. Type: %hhu", header.type);
        dump(data, header.length - sizeof(Message::RFSHeader));

        return;
    }

    Message *message = Messages::Factory::create(Messages::CMD_RFS, header.type);

    if(message == NULL) {
        Log::error("Unknown RFS message received. Type: %hhu", header.type);

        return;
    }

    std::vector<unsigned char> buffer((unsigned char *) data,
                                      (unsigned char *) data + header.length - sizeof(Message::RFSHeader));

    DataStream stream(&buffer, DataStream::Read);
    if(!message->readFromStream(stream) || !stream.atEnd()) {
        Log::error("RFS Message demarshalling failed. Type: %hhu", header.type);

        delete message;

        return;
    }

#if defined(PACKET_INSPECTION)
    Log::debug("Received request: %s", message->inspect().c_str());
#endif

    Message *response = (this->*m_handlers[header.type - FirstType])(message);
    delete message;

    if(response) {
        std::vector<unsigned char> data;
        DataStream stream(&data, DataStream::Write);

        if(!response->writeToStream(stream)) {
            Log::error("Response marshalling failed. Type: %hhu", header.type);

            delete response;

            return;
        }

        Message::RFSHeader responseHeader;
        responseHeader.length = sizeof(Message::RFSHeader) + data.size();
        responseHeader.type = response->subcommand();
        responseHeader.sequence = header.sequence;

#if defined(PACKET_INSPECTION)
        Log::debug("Sending response: %s", response->inspect().c_str());
#endif

        sendMessage(responseHeader, &data[0]);

        delete response;
    }
}

size_t RFSSocketHandler::headerSize() {
    return sizeof(Message::RFSHeader);
}

size_t RFSSocketHandler::messageSize(const unsigned char *data) {
    return reinterpret_cast<const Message::RFSHeader *>(data)->length;
}

void RFSSocketHandler::handleReassembledMessage(const unsigned char *data) {
    const Message::RFSHeader *header = reinterpret_cast<const Message::RFSHeader *>(data);

    handleMessage(*header, header + 1);
}

void RFSSocketHandler::sendMessage(const Message::RFSHeader &header,
                                   const void *data) {

    unsigned char *buf = new unsigned char[header.length];

    memcpy(buf, &header, sizeof(Message::RFSHeader));
    memcpy(buf + sizeof(Message::RFSHeader), data, header.length -
                                                   sizeof(Message::RFSHeader));

    // 'Ready for write' event and writev call aren't implemented in kernel driver.

    sendData(buf, header.length);
    delete[] buf;
}

Message *RFSSocketHandler::handleNvRead(Message *msg) {
    Messages::RfsNvRead *nvRead = static_cast<Messages::RfsNvRead *>(msg);

    RFS_TRACE("reading %u bytes from NV offset 0x%08X", nvRead->size(), nvRead->offset());

    std::vector<unsigned char> data;
    size_t len = std::min(512U, nvRead->size());
    data.reserve(512);
    data.resize(len);

    int ret = m_fs->readNVData(nvRead->offset(), data);

    RFS_TRACE("status %d", ret);

    data.resize(512);

    Messages::RfsNvReadReply *reply = new Messages::RfsNvReadReply;
    reply->setOffset(nvRead->offset());
    reply->setSize(nvRead->size());
    reply->setStatus(ret == 0);
    reply->setData(data);

    return reply;
}

Message *RFSSocketHandler::handleNvWrite(Message *msg) {
    Messages::RfsNvWrite *nvWrite = static_cast<Messages::RfsNvWrite *>(msg);

    RFS_TRACE("writing %u bytes to NV offset 0x%08X", nvWrite->size(), nvWrite->offset());

    int ret = m_fs->writeNVData(nvWrite->offset(), nvWrite->data());

    RFS_TRACE("status %d", ret);

    Messages::RfsNvWriteReply *reply = new Messages::RfsNvWriteReply;
    reply->setOffset(nvWrite->offset());
    reply->setSize(nvWrite->size());
    reply->setStatus(ret == 0);

    return reply;
}

Message *RFSSocketHandler::handleReadFile(Message *msg) {
    Messages::RfsReadFile *readFile = static_cast<Messages::RfsReadFile *>(msg);

    RFS_TRACE("reading %u bytes from fd %d", readFile->bytes(), readFile->fd());

    std::vector<unsigned char> data;
    data.reserve(4096);
    data.resize(std::min(4096U, readFile->bytes()));

    int ret = m_fs->readFile(readFile->fd(), data);
    RFS_TRACE("status %d", ret);

    data.resize(4096);

    Messages::RfsReadFileReply *reply = new Messages::RfsReadFileReply;
    buildReply(reply, ret);
    reply->setData(data);

    return reply;
}

Message *RFSSocketHandler::handleWriteFile(Message *msg) {
    Messages::RfsWriteFile *writeFile = static_cast<Messages::RfsWriteFile *>(msg);

    RFS_TRACE("writing %u bytes to fd %d", writeFile->bytes(), writeFile->fd());

    int ret = m_fs->writeFile(writeFile->fd(), writeFile->data());

    RFS_TRACE("status %d", ret);

    Messages::RfsWriteFileReply *reply = new Messages::RfsWriteFileReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handleLseekFile(Message *msg) {
    Messages::RfsLseekFile *lseekFile = static_cast<Messages::RfsLseekFile *>(msg);

    RFS_TRACE("seeking %d to %u from %d", lseekFile->fd(), lseekFile->offset(), lseekFile->whence());

    int ret = m_fs->lseekFile(lseekFile->fd(), lseekFile->offset(), lseekFile->whence());

    RFS_TRACE("status %d", ret);

    Messages::RfsLseekFileReply *reply = new Messages::RfsLseekFileReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handleCloseFile(Message *msg) {
    Messages::RfsCloseFile *closeFile = static_cast<Messages::RfsCloseFile *>(msg);

    RFS_TRACE("closing file %d", closeFile->fd());

    int ret = m_fs->closeFile(closeFile->fd());

    RFS_TRACE("status %d", ret);

    Messages::RfsCloseFileReply *reply = new Messages::RfsCloseFileReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handlePutFile(Message *msg) {
    Messages::RfsPutFile *putFile = static_cast<Messages::RfsPutFile *>(msg);

    std::string name;
    name.assign((char *) &putFile->name()[0], std::min(putFile->name().size(), putFile->nameSize()));

    RFS_TRACE("putting %u bytes to %s (mode %08X)", putFile->bytes(), name.c_str(), putFile->flags());

    int ret = m_fs->openFile(name, putFile->flags());

    if(ret >= 0) {
        int fd = ret;

        ret = m_fs->writeFile(fd, putFile->data());

        m_fs->closeFile(fd);
    }

    RFS_TRACE("status %d", ret);

    Messages::RfsPutFileReply *reply = new Messages::RfsPutFileReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handleGetFile(Message *msg) {
    Messages::RfsGetFile *getFile = static_cast<Messages::RfsGetFile *>(msg);

    std::string name;
    name.assign((char *) &getFile->name()[0], std::min(getFile->name().size(), getFile->nameSize()));

    RFS_TRACE("getting %u bytes from %s", getFile->size(), name.c_str());

    std::vector<unsigned char> data;
    data.reserve(4096);
    data.resize(std::min(4096U, getFile->size()));

    int ret = m_fs->openFile(name, O_RDONLY);

    if(ret >= 0) {
        int fd = ret;

        ret = m_fs->readFile(fd, data);

        m_fs->closeFile(fd);
    }

    RFS_TRACE("status %d", ret);

    data.resize(4096);

    Messages::RfsGetFileReply *reply = new Messages::RfsGetFileReply;
    buildReply(reply, ret);
    reply->setData(data);

    return reply;
}

Message *RFSSocketHandler::handleRenameFile(Message *msg) {
    Messages::RfsRenameFile *renameFile = static_cast<Messages::RfsRenameFile *>(msg);

    std::string oldName, newName;
    oldName.assign((char *) &renameFile->oldName()[0], std::min(renameFile->oldName().size(), renameFile->oldNameSize()));
    newName.assign((char *) &renameFile->newName()[0], std::min(renameFile->newName().size(), renameFile->newNameSize()));

    RFS_TRACE("renaming %s to %s", oldName.c_str(), newName.c_str());

    int ret = m_fs->renameFile(oldName, newName);

    RFS_TRACE("status %d", ret);

    Messages::RfsRenameFileReply *reply = new Messages::RfsRenameFileReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handleGetFileInfo(Message *msg) {
    Messages::RfsGetFileInfo *getFileInfo = static_cast<Messages::RfsGetFileInfo *>(msg);

    std::string name;
    name.assign((char *) &getFileInfo->name()[0], std::min(getFileInfo->name().size(), getFileInfo->nameSize()));

    RFS_TRACE("getting %s info", name.c_str());

    struct stat statbuf;
    int ret = m_fs->openFile(name.c_str(), O_RDONLY);
    if(ret >= 0) {
        int fd = ret;

        ret = m_fs->getFileInfoByHandle(fd, &statbuf);
        m_fs->closeFile(fd);
    }

    RFS_TRACE("status %d", ret);

    Messages::RfsGetFileInfoReply *reply = new Messages::RfsGetFileInfoReply;
    buildReply(reply, ret);
    buildFileInfo(reply, statbuf);

    return reply;
}

Message *RFSSocketHandler::handleDeleteFile(Message *msg) {
    Messages::RfsDeleteFile *deleteFile = static_cast<Messages::RfsDeleteFile *>(msg);

    std::string name;
    name.assign((char *) &deleteFile->name()[0], std::min(deleteFile->name().size(), deleteFile->nameSize()));

    RFS_TRACE("deleting %s", name.c_str());

    int ret = m_fs->deleteFile(name);

    RFS_TRACE("status %d", ret);

    Messages::RfsDeleteFileReply *reply = new Messages::RfsDeleteFileReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handleMakeDirectory(Message *msg) {
    Messages::RfsMakeDirectory *makeDirectory = static_cast<Messages::RfsMakeDirectory *>(msg);

    std::string name;
    name.assign((char *) &makeDirectory->name()[0], std::min(makeDirectory->name().size(), makeDirectory->nameSize()));

    RFS_TRACE("creating directory %s", name.c_str());

    int ret = m_fs->makeDirectory(name);

    Messages::RfsMakeDirectoryReply *reply = new Messages::RfsMakeDirectoryReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handleDeleteDirectory(Message *msg) {
    Messages::RfsDeleteDirectory *deleteDirectory = static_cast<Messages::RfsDeleteDirectory *>(msg);

    std::string name;
    name.assign((char *) &deleteDirectory->name()[0], std::min(deleteDirectory->name().size(), deleteDirectory->nameSize()));

    RFS_TRACE("deleting directory %s", name.c_str());

    int ret = m_fs->deleteDirectory(name);

    Messages::RfsDeleteDirectoryReply *reply = new Messages::RfsDeleteDirectoryReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handleOpenDirectory(Message *msg) {
    Messages::RfsOpenDirectory *openDirectory = static_cast<Messages::RfsOpenDirectory *>(msg);

    std::string name;
    name.assign((char *) &openDirectory->name()[0], std::min(openDirectory->name().size(), openDirectory->nameSize()));

    RFS_TRACE("opening directory %s", name.c_str());

    int ret = m_fs->openDirectory(name);

    Messages::RfsOpenDirectoryReply *reply = new Messages::RfsOpenDirectoryReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handleReadDirectory(Message *msg) {
    Messages::RfsReadDirectory *readDirectory = static_cast<Messages::RfsReadDirectory *>(msg);

    RFS_TRACE("reading directory %d", readDirectory->fd());

    std::string name;

    int ret = m_fs->readDirectory(readDirectory->fd(), name);

    std::vector<unsigned char> data;
    data.resize(1024);

    Messages::RfsReadDirectoryReply *reply = new Messages::RfsReadDirectoryReply;

    if(ret == 1) {
        size_t len = std::min(data.size(), name.length());

        reply->setNameLength(len);
        memcpy(&data[0], name.c_str(), len);

        reply->setRet(0);
        reply->setErrno(0);
    } else {
        reply->setNameLength(0);
        reply->setRet(-1);
        reply->setErrno(-ret);
    }

    reply->setName(data);

    return reply;
}

Message *RFSSocketHandler::handleCloseDirectory(Message *msg) {
    Messages::RfsCloseDirectory *closeDirectory = static_cast<Messages::RfsCloseDirectory *>(msg);

    RFS_TRACE("closing directory %d", closeDirectory->fd());

    int ret = m_fs->closeDirectory(closeDirectory->fd());

    RFS_TRACE("status %d", ret);

    Messages::RfsCloseDirectoryReply *reply = new Messages::RfsCloseDirectoryReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handleOpenFile(Message *msg) {
    Messages::RfsOpenFile *openFile = static_cast<Messages::RfsOpenFile *>(msg);

    std::string name;
    name.assign((char *) &openFile->name()[0], std::min(openFile->name().size(), openFile->nameSize()));

    RFS_TRACE("opening file %s, flags 0x%08X", name.c_str(), openFile->flags());

    int ret = m_fs->openFile(name, openFile->flags());

    RFS_TRACE("status %d", ret);

    Messages::RfsOpenFileReply *reply = new Messages::RfsOpenFileReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handleFtruncateFile(Message *msg) {
    Messages::RfsFtruncateFile *ftruncateFile = static_cast<Messages::RfsFtruncateFile *>(msg);

    RFS_TRACE("truncating file %d to %u bytes", ftruncateFile->fd(), ftruncateFile->size());

    int ret = m_fs->ftruncateFile(ftruncateFile->fd(), ftruncateFile->size());

    RFS_TRACE("status %d", ret);

    Messages::RfsFtruncateFileReply *reply = new Messages::RfsFtruncateFileReply;
    buildReply(reply, ret);

    return reply;
}

Message *RFSSocketHandler::handleGetFileInfoByHandle(Message *msg) {
    Messages::RfsGetFileInfoByHandle *getFileInfoByHandle = static_cast<Messages::RfsGetFileInfoByHandle *>(msg);

    RFS_TRACE("getting %d info", getFileInfoByHandle->fd());

    struct stat statbuf;
    int ret = m_fs->getFileInfoByHandle(getFileInfoByHandle->fd(), &statbuf);

    RFS_TRACE("status %d", ret);

    Messages::RfsGetFileInfoByHandleReply *reply = new Messages::RfsGetFileInfoByHandleReply;
    buildReply(reply, ret);
    buildFileInfo(reply, statbuf);

    return reply;
}

Message *RFSSocketHandler::handleNvWriteAll(Message *msg) {
    Messages::RfsNvWriteAll *nvWriteAll = static_cast<Messages::RfsNvWriteAll *>(msg);

    RFS_TRACE("writing %u bytes to NV offset 0x%08X", nvWriteAll->size(), nvWriteAll->offset());

    int ret = m_fs->writeNVData(nvWriteAll->offset(), nvWriteAll->data());

    RFS_TRACE("status %d", ret);

    Messages::RfsNvWriteAllReply *reply = new Messages::RfsNvWriteAllReply;
    reply->setOffset(nvWriteAll->offset());
    reply->setSize(nvWriteAll->size());
    reply->setStatus(ret == 0);

    return reply;
}

Message *(RFSSocketHandler::*const RFSSocketHandler::m_handlers[LastType - FirstType + 1])(Message *message) = {
    &RFSSocketHandler::handleNvRead,
    &RFSSocketHandler::handleNvWrite,
    &RFSSocketHandler::handleReadFile,
    &RFSSocketHandler::handleWriteFile,
    &RFSSocketHandler::handleLseekFile,
    &RFSSocketHandler::handleCloseFile,
    &RFSSocketHandler::handlePutFile,
    &RFSSocketHandler::handleGetFile,
    &RFSSocketHandler::handleRenameFile,
    &RFSSocketHandler::handleGetFileInfo,
    &RFSSocketHandler::handleDeleteFile,
    &RFSSocketHandler::handleMakeDirectory,
    &RFSSocketHandler::handleDeleteDirectory,
    &RFSSocketHandler::handleOpenDirectory,
    &RFSSocketHandler::handleReadDirectory,
    &RFSSocketHandler::handleCloseDirectory,
    &RFSSocketHandler::handleOpenFile,
    &RFSSocketHandler::handleFtruncateFile,
    &RFSSocketHandler::handleGetFileInfoByHandle,
    NULL,
    &RFSSocketHandler::handleNvWriteAll
};

