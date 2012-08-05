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

    g.in :PIN_STATUS, 1 do |m|
        m.u8 :status, :type => :enum, :values => {
            SimAbsent: 128
        }
        # if absent:
        # m.u8 rsvd

    end
    #g.unsolicited :PIN_STATUS

    g.in :SIM_CARD_TYPE, 7 do |m|
        m.u8 :card_type, :type => :enum, :values => {
            Absent: 0
        }
        m.u8 :icc_type, :if => "m_card_type == 3", :else => "0"
    end

    #g.unsolicited :SIM_CARD_TYPE

end
