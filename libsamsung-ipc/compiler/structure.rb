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

class Structure
    attr_accessor :fields

    def initialize
        @fields = []
    end

    [ :data, :u8, :u16, :u32, :vector ].each do |type|
        define_method(type) do |name, parameters = {}|
            @fields << Field.new(type, name, parameters)
        end
    end

    def array(name, parameters, &block)
        structure = Structure.new
        structure.instance_exec(structure, &block)

        parameters = parameters.dup
        parameters[:structure] = structure

        @fields << Field.new(:array, name, parameters)
    end
end
