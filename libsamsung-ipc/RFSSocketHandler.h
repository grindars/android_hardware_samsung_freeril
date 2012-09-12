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

#ifndef __RFS_SOCKET_HANDLER__H__
#define __RFS_SOCKET_HANDLER__H__

#include "MessageTypes.h"
#include "SocketHandler.h"

namespace SamsungIPC {
    class IFileSystem;

    class RFSSocketHandler: public SocketHandler {
    public:
        RFSSocketHandler(IIPCSocket *socket, IFileSystem *fs);

    protected:
        void handleMessage(const Message::RFSHeader &header,
                           const void *data);
        void sendMessage(const Message::RFSHeader &header, const void *data);
        virtual size_t headerSize();
        virtual size_t messageSize(const unsigned char *data);
        virtual void handleReassembledMessage(const unsigned char *data);

    private:
        Message *handleNvRead(Message *msg);
        Message *handleNvWrite(Message *msg);
        Message *handleReadFile(Message *msg);
        Message *handleWriteFile(Message *msg);
        Message *handleLseekFile(Message *msg);
        Message *handleCloseFile(Message *msg);
        Message *handlePutFile(Message *msg);
        Message *handleGetFile(Message *msg);
        Message *handleRenameFile(Message *msg);
        Message *handleGetFileInfo(Message *msg);
        Message *handleDeleteFile(Message *msg);
        Message *handleMakeDirectory(Message *msg);
        Message *handleDeleteDirectory(Message *msg);
        Message *handleOpenDirectory(Message *msg);
        Message *handleReadDirectory(Message *msg);
        Message *handleCloseDirectory(Message *msg);
        Message *handleOpenFile(Message *msg);
        Message *handleFtruncateFile(Message *msg);
        Message *handleGetFileInfoByHandle(Message *msg);
        Message *handleNvWriteAll(Message *msg);

        enum {
            FirstType = Messages::RFS_NV_READ,
            LastType  = Messages::RFS_NV_WRITE_ALL
        };

        static Message *(RFSSocketHandler::*const m_handlers[LastType - FirstType + 1])(Message *message);
        IFileSystem *m_fs;
    };
}

#endif
