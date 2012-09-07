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

message_group :SVC, 11 do |g|

    g.out :ENTER_SERVICE_MODE, 1, :set do |m|
        m.u8  :modeType
        m.u16 :subType
    end

    g.out :EXIT_SERVICE_MODE, 2, :set do |m|
        m.u8 :modeType
    end

    g.out :PROCESS_KEY_CODE, 3, :set do |m|
        m.u8 :keyCode
    end

    g.in :DISPLAY_SCREEN, 5 do |m|
        m.u8 :lineCount
        m.array :lines, :read_length => "m_lineCount" do |a|
            a.u8     :unknown1
            a.u8     :unknown2
            a.vector :line, :read_length => 32
        end
    end
    g.unsolicited :DISPLAY_SCREEN
end
