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

class MessageGroup
    attr_accessor :id, :type, :messages, :bindings, :unsolicited_messages

    def initialize(id, type)
        @id = id
        @type = type
        @messages = []
        @bindings = {}
        @unsolicited_messages = []
    end

    def out(id, type, &block)
        message = Message.new :out, id, type

        yield message if block_given?

        @messages << message
    end

    def in(id, type, &block)
        message = Message.new :in, id, type

        yield message if block_given?

        @messages << message
    end

    def bind(request, reply)
        @bindings[request] = reply
    end

    def unsolicited(reply)
        @unsolicited_messages << reply
    end
end
