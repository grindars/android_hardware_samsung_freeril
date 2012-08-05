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

class Message
    attr_accessor :direction, :id, :type, :fields, :op

    def initialize(direction, id, type, op)
        @direction = direction
        @id = id
        @type = type
        @op = op
        @fields = []
    end

    def data(name, *parameters)
        @fields << Field.new(:data, name, parameters[0] || {})
    end

    def u8(name, *parameters)
        @fields << Field.new(:u8, name, parameters[0] || {})
    end

    def u16(name, *parameters)
        @fields << Field.new(:u16, name, parameters[0] || {})
    end

    def u32(name, *parameters)
        @fields << Field.new(:u32, name, parameters[0] || {})
    end

    def vector(name, *parameters)
        @fields << Field.new(:vector, name, parameters[0] || {})
    end
end