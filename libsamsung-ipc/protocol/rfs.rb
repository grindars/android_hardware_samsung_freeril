#
# Free RIL implementation for Samsung Android-based smartphones.
# Copyright (C) 2012  Sergey Gridasov <grindars@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

message_group :RFS, 21 do |g|

    g.in :NV_READ, 1 do |m|
        m.u32    :offset
        m.u32    :size
    end

    g.out :NV_READ_REPLY, 1 do |m|
        m.u8     :status
        m.u32    :offset
        m.u32    :size
        m.vector :data, :read_length => 512
    end

    g.in :NV_WRITE, 2 do |m|
        m.u32    :offset
        m.u32    :size
        m.vector :data, :read_length => "m_size"
    end

    g.out :NV_WRITE_REPLY, 2 do |m|
        m.u8  :status
        m.u32 :offset
        m.u32 :size
    end

    g.in :READ_FILE, 3 do |m|
        m.u32   :fd
        m.u32   :bytes
    end

    g.out :READ_FILE_REPLY, 3 do |m|
        m.u32    :ret
        m.u32    :errno
        m.vector :data, :read_length => 4096
    end

    g.in :WRITE_FILE, 4 do |m|
        m.u32    :fd
        m.u32    :bytes
        m.vector :data, :read_length => "m_bytes"
    end

    g.out :WRITE_FILE_REPLY, 4 do |m|
        m.u32    :ret
        m.u32    :errno
    end

    g.in :LSEEK_FILE, 5 do |m|
        m.u32   :fd
        m.u32   :offset
        m.u32   :whence
    end

    g.out :LSEEK_FILE_REPLY, 5 do |m|
        m.u32    :ret
        m.u32    :errno
    end

    g.in :CLOSE_FILE, 6 do |m|
        m.u32    :fd
    end

    g.out :CLOSE_FILE_REPLY, 6 do |m|
        m.u32    :ret
        m.u32    :errno
    end

    g.in :PUT_FILE, 7 do |m|
        m.u32    :flags
        m.u32    :nameSize
        m.vector :name, :read_length => 1024
        m.u32    :bytes
        m.vector :data, :read_length => "m_bytes"
    end

    g.out :PUT_FILE_REPLY, 7 do |m|
        m.u32    :ret
        m.u32    :errno
    end

    g.in :GET_FILE, 8 do |m|
        m.u32    :nameSize
        m.vector :name, :read_length => 1024
        m.u32    :size
    end

    g.out :GET_FILE_REPLY, 8 do |m|
        m.u32    :ret
        m.u32    :errno
        m.vector :data, :read_length => 4096
    end

    g.in :RENAME_FILE, 9 do |m|
        m.u32    :oldNameSize
        m.vector :oldName, :read_length => 1024

        m.u32    :newNameSize
        m.vector :newName, :read_length => 1024
    end

    g.out :RENAME_FILE_REPLY, 9 do |m|
        m.u32    :ret
        m.u32    :errno
    end

    g.in :GET_FILE_INFO, 10 do |m|
        m.u32    :nameSize
        m.vector :name, :read_length => 1024
    end

    g.out :GET_FILE_INFO_REPLY, 10 do |m|
        m.u32    :ret

        m.u16   :type
        m.u32   :size

        m.u8    :cyear
        m.u8    :cmonth
        m.u8    :cday
        m.u8    :chour
        m.u8    :cminute
        m.u8    :csecond

        m.u8    :myear
        m.u8    :mmonth
        m.u8    :mday
        m.u8    :mhour
        m.u8    :mminute
        m.u8    :msecond

        m.u32    :errno
    end

    g.in :DELETE_FILE, 11 do |m|
        m.u32    :nameSize
        m.vector :name, :read_length => 1024
    end

    g.out :DELETE_FILE_REPLY, 11 do |m|
        m.u32    :ret
        m.u32    :errno
    end

    g.in :MAKE_DIRECTORY, 12 do |m|
        m.u32    :nameSize
        m.vector :name, :read_length => 1024
    end

    g.out :MAKE_DIRECTORY_REPLY, 12 do |m|
        m.u32   :ret
        m.u32   :errno
    end

    g.in :DELETE_DIRECTORY, 13 do |m|
        m.u32    :nameSize
        m.vector :name, :read_length => 1024
    end

    g.out :DELETE_DIRECTORY_REPLY, 13 do |m|
        m.u32   :ret
        m.u32   :errno
    end

    g.in :OPEN_DIRECTORY, 14 do |m|
        m.u32    :nameSize
        m.vector :name, :read_length => 1024
    end

    g.out :OPEN_DIRECTORY_REPLY, 14 do |m|
        m.u32   :ret
        m.u32   :errno
    end

    g.in :READ_DIRECTORY, 15 do |m|
        m.u32   :fd
    end

    g.out :READ_DIRECTORY_REPLY, 15 do |m|
        m.u32    :ret
        m.u32    :nameLength
        m.vector :name, :read_length => 1024
        m.u32    :errno
    end

    g.in :CLOSE_DIRECTORY, 16 do |m|
        m.u32   :fd
    end

    g.out :CLOSE_DIRECTORY_REPLY, 16 do |m|
        m.u32   :ret
        m.u32   :errno
    end

    g.in :OPEN_FILE, 17 do |m|
        m.u32    :flags
        m.u32    :nameSize
        m.vector :name, :read_length => 1024
    end

    g.out :OPEN_FILE_REPLY, 17 do |m|
        m.u32   :ret
        m.u32   :errno
    end

    g.in :FTRUNCATE_FILE, 18 do |m|
        m.u32   :fd
        m.u32   :size
    end

    g.out :FTRUNCATE_FILE_REPLY, 18 do |m|
        m.u32   :ret
        m.u32   :errno
    end

    g.in :GET_FILE_INFO_BY_HANDLE, 19 do |m|
        m.u32   :fd
    end

    g.out :GET_FILE_INFO_BY_HANDLE_REPLY, 19 do |m|
        m.u32    :ret

        m.u16   :type
        m.u32   :size

        m.u8    :cyear
        m.u8    :cmonth
        m.u8    :cday
        m.u8    :chour
        m.u8    :cminute
        m.u8    :csecond

        m.u8    :myear
        m.u8    :mmonth
        m.u8    :mday
        m.u8    :mhour
        m.u8    :mminute
        m.u8    :msecond

        m.u32    :errno
    end

    g.in :NV_WRITE_ALL, 21 do |m|
        m.u32    :offset
        m.u32    :size
        m.vector :data, :read_length => "m_size"
    end

    g.out :NV_WRITE_ALL_REPLY, 21 do |m|
        m.u8  :status
        m.u32 :offset
        m.u32 :size
    end
end
