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

message_group :MISC, 10 do |g|
    g.out :GET_MOBILE_EQUIP_VERSION, 1, :get do |m|
        m.u8 :reserved # set to 0xFF
    end

    g.in :GET_MOBILE_EQUIP_VERSION_REPLY, 1 do |m|
        m.u8 :unused1 # 0x1F

        m.vector :softwareVersion, :read_length => 32
        m.vector :hardwareVersion, :read_length => 32
        m.vector :RFCalDate, :read_length => 32
        m.vector :productCode, :read_length => 32
        m.vector :model, :read_length => 17
        m.u8     :PRLNum

        m.u8 :unused2 # 0x00
    end

    g.out :SET_DEBUG_LEVEL, 2, :event do |m|
        m.u8 :level
    end

    g.out :GET_MOBILE_EQUIP_IMSI, 2, :get

    g.in :GET_MOBILE_EQUIP_IMSI_REPLY, 2 do |m|
        m.u8 :stringLength # 0x0F
        m.vector :imsi, :read_length => "m_stringLength"
    end
    g.unsolicited :GET_MOBILE_EQUIP_IMSI_REPLY

    g.out :GET_MOBILE_EQUIP_SERIAL_NUMBER, 3, :get do |m|
        m.u8 :reserved # set to 0x01
    end

    g.in :GET_MOBILE_EQUIP_SERIAL_NUMBER_REPLY, 3 do |m|
        m.u8     :reserved # 0x01

        m.u8     :stringLength # 0x11
        m.vector :serial, :read_length => 32
    end


end
