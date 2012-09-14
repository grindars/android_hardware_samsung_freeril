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

message_group :SEC, 5 do |g|
    g.out :SET_PIN_STATUS, 1, :set do |m|
        m.u8 :type, :type => :enum, :values => {
            Pin:  0x03,
            Pin2: 0x09
        }

        m.u8 :pin1Length
        m.u8 :pin2Length
        m.vector :pin1, :read_length => 8
        m.vector :pin2, :read_length => 8
    end

    g.out :GET_PIN_STATUS, 1, :get
    g.in :GET_PIN_STATUS_REPLY, 1 do |m|
        m.u8 :state, :type => :enum, :values => {
            Ready:          0x00,
            PINLock:        0x03,
            PNLock:         0x05,
            PULock:         0x06,
            PPLock:         0x07,
            PCLock:         0x08,
            NoSimPresent:   0x80,
            SimUnavailable: 0x81
        }

        m.u8 :xstate

        # PINLock && xstate != 0x01 -> PUKLock
        # + special handling for PN/PU/PP/PC Lock
    end
    g.unsolicited :GET_PIN_STATUS_REPLY

    g.out :CHANGE_LOCK_PWD, 3, :set do |m|
        m.u8 :type, :type => :enum, :values => {
            Pin:  0x03,
            Pin2: 0x09
        }

        m.u8 :currentLength
        m.u8 :newLength

        m.vector :currentPwd, :read_length => 39
        m.vector :newPwd,     :read_length => 39
    end

    g.out :RSIM_ACCESS, 5, :get do |m|
        m.u8     :cmd
        m.u16    :fileId
        m.u8     :p1
        m.u8     :p2
        m.u8     :p3
        m.vector :data, :read_length => 256
    end

    g.in :RSIM_ACCESS_REPLY, 5 do |m|
        m.u8     :sw1
        m.u8     :sw2
        m.u8     :dataLength
        m.vector :data, :read_length => "m_dataLength"

    end

    g.in :SIM_CARD_TYPE, 7 do |m|
        m.u8 :cardType
        m.u8 :iccType
    end
    g.unsolicited :SIM_CARD_TYPE

end
