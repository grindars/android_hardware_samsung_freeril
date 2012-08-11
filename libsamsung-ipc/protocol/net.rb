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

message_group :NET, 8 do |g|
    g.out :GET_PLMN_SELECTION, 2, :get
    g.in :GET_PLMN_SELECTION_REPLY, 2 do |m|
        m.u8 :selection
    end

    g.out :SET_PLMN_SELECTION, 2, :set do |m|
        m.u8 :selection, :type => :enum, :values => {
            Automatic: 2,
            Manual:    3
        }
        m.vector :network, :read_length => 6
        m.u8 :unknown1 # always 0xFF
    end

    g.out :GET_CURRENT_PLMN, 3, :get
    g.in :GET_CURRENT_PLMN_REPLY, 3 do |m|
        m.u8 :unknown1 # Must be 0x02
        m.u16 :unknown2
        m.vector :plmn, :read_length => 6
        m.u16 :unknown3
    end

    g.out :GET_PLMN_LIST, 4, :get
    g.in :GET_PLMN_LIST_REPLY, 4 do |m|
        m.u8 :count
        m.vector :data, :read_length => "stream.remainingBytes()"

        # for each PLMN: (10 bytes)
        # +0 u8 :status, :type => enum, :values = {
        #        Available: 2
        #        Current:   3,
        #        Forbidden: 4,
        #    }
        # +1 vector :plmn, :read_length => 6
        # +7 u8 :unknown2
        # +8 u8 :unknown3
        # +9 u8 :unknown4
    end
end
