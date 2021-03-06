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

message_group :GEN, 128 do |g|

    g.in :COMMAND_COMPLETE, 1 do |m|
        # Original command header
        m.u8 :originalCommand
        m.u8 :originalSubcommand
        m.u8 :originalResponseType

        m.u16 :status, :type => :enum, :values => {
            IncorrectPin: 0x0010,
            Success:      0x8000
        }
    end
end
