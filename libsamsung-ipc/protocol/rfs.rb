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
        m.u32    :bytes
    end

    g.out :NV_READ_REPLY, 1 do |m|
        m.u8  :status
        m.u32 :offset
        m.u32 :bytes
        m.vector :data, :read_length => 0
    end

    # Header takes 6 bytes.

    g.in :NV_WRITE, 2 do |m|
        m.u32    :offset
        m.u32    :bytes, :initialize => "m_data.size()"
        m.vector :data, :read_length => "m_bytes"
    end

    g.out :NV_WRITE_REPLY, 2 do |m|
        m.u8  :status
        m.u32 :offset
        m.u32 :bytes
    end

end
