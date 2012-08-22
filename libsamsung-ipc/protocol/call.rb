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

message_group :CALL, 2 do |g|
    g.out :ORIGINATE, 1, :exec do |m|
        m.u16 :type, :type => :enum, :values => {
            Normal:    0x100,
            Video:     0x300,
            Emergency: 0x700
        }
        m.u8 :allowCLI, :type => :enum, :values => {
            Default:   0x00,
            Restrict:  0x01,
            Allow:     0x02
        }

        m.u8 :numberLength
        m.u8 :numberType, :type => :enum, :values => {
            International: 0x11,
            Unknown:       0x21
        }
        m.vector :number, :read_length => 82

        m.u32 :reserved # Always zero
        m.u8  :extension
    end

    g.in :INCOMING, 2 do |m|
        m.u16 :callType
        m.u8 :reserved # Not used by the RIL
        m.u8 :lineId
    end
    g.unsolicited :INCOMING

    g.in :STATE_CHANGED, 5 do |m|
        m.u16 :callType
        m.u8  :callId
        m.u8  :event
        m.u8  :releaseCause1
        m.u8  :releaseCause2
    end
    g.unsolicited :STATE_CHANGED

    g.out :GET_CALL_LIST, 6, :get
    g.in :GET_CALL_LIST_REPLY, 6 do |m|
        m.u8 :count

        m.array :callList, :read_length => "m_count" do |a|
            a.u16    :callType
            a.u8     :callId
            a.u8     :direction, :type => :enum, :values => {
                MO: 1,
                MT: 2
            }
            a.u8     :callState
            a.u8     :isMultiParty

            a.u8     :numberLength
            a.u8     :reserved # Not used by the RIL
            a.vector :number, :read_length => "m_numberLength"
        end

        m.vector :padding, :read_length => "stream.remainingBytes()"
    end
end
