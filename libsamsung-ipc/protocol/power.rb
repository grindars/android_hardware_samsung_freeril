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

message_group :PWR, 1 do |g|
    g.out :PHONE_POWER_ON, 1 do |m|
        # 8+ kilobytes of zeros.
        m.data :reserved, :size => 8196
    end

    g.in :PHONE_BOOT_COMPLETE, 1 do |m|
        m.u8 :type, :type => :enum, :values => {
            Normal: 0
        }
    end

    g.unsolicited :PHONE_BOOT_COMPLETE

    g.out :PHONE_POWER_OFF, 2

    g.in :PHONE_POWERED_OFF, 2 do |m|
        m.u8 :reason, :type => :enum, :values => {
            LowBattery: 0
        }
    end

    g.unsolicited :PHONE_POWERED_OFF

    g.in :PHONE_RESET, 3
    g.unsolicited :PHONE_RESET

    g.out :PHONE_SET_MODE, 7 do |m|
        m.u8 :mode, :type => :enum, :values => {
            LPM:    1,
            Normal: 2
        }

        # Set to 0x02 for normal, zero for LPM.
        m.u8 :flags
    end
    g.in :PHONE_MODE_CHANGED, 7 do |m|
        m.u8 :mode, :type => :enum, :values => {
            LPM:    1,
            Normal: 2
        }
    end
    g.unsolicited :PHONE_MODE_CHANGED

end
