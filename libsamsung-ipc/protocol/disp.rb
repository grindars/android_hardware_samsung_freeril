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

message_group :DISP, 7 do |g|
    g.out :GET_ICON_INFORMATION, 1, :get do |m|
        m.u8 :icon, :type => :enum, :values => {
            SignalStrength: 1
        }
    end
    g.in :GET_ICON_INFORMATION_REPLY, 1 do |m|
        m.u8 :icon
        m.u8 :value
        m.u16 :unknown1 # Unused by RIL
    end
    g.unsolicited :GET_ICON_INFORMATION_REPLY

    g.in :RSSI_INFORMATION, 6 do |m|
        m.u8 :rssi
    end
    g.unsolicited :RSSI_INFORMATION
end
