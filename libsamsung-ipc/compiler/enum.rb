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

class Enum
    attr_accessor :name, :values

    def initialize(name = nil, values = [])
        @name = name
        @values = values
    end

    def add_minmax!(prefix = "")
        min, max = values.minmax { |a, b| a[1] <=> b[1] }

        values << [ prefix + "FIRST", min[1] ]
        values << [ prefix + "LAST", max[1] ]
    end
end
