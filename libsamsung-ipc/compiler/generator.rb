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
class GeneratorBinding
    def class_name_for(message_name)
        message_name.split("_").map(&:capitalize).join
    end

    def fieldize(s)
        s[0].upcase + s[1..-1]
    end
end

class Generator

    STORAGE_TYPE_MAP = {
        u8: 'uint8_t',
        u16: 'uint16_t',
        u32: 'uint32_t'
    }

    REQUEST_TYPE_MAP = {
        exec:  "IPC_CMD_EXEC",
        get:   "IPC_CMD_GET",
        set:   "IPC_CMD_SET",
        cfrm:  "IPC_CMD_CFRM",
        event: "IPC_CMD_EVENT"
    }

    def initialize(destination)
        @destination = destination
        @templates = {}
    end

    def implement(protocol)
        enums = generate_enums(protocol)

        params = {
            :@protocol => protocol,
            :@enums    => enums
        }

        write_file("header.erb", "MessageTypes.h")         { template "MessageTypes.h.erb", params }
        write_file("header.erb", "MessageFactory.h")       { template "MessageFactory.h.erb", params }
        write_file("source.erb", "MessageFactory.cpp")     { template "MessageFactory.cpp.erb", params }
        write_file("header.erb", "IUnsolicitedReceiver.h") { template "IUnsolicitedReceiver.h.erb", params }
        write_file("header.erb", "Messages.h")             { template "Messages.h.erb", params }
        write_file("source.erb", "Messages.cpp")           { template "Messages.cpp.erb", params }
    end

    def template(name, parameters = {})
        template = get_template name

        obj = GeneratorBinding.new

        parameters.each { |key, value| obj.instance_variable_set key, value }
        obj.instance_variable_set :@generator, self
        obj_binding = obj.instance_exec { binding }

        template.result obj_binding
    end

    protected

    def get_template(name)
        template = @templates[name]

        if template.nil?
            pathname = File.join File.dirname(__FILE__), "templates", name

            template = ERB.new File.read(pathname), nil
            template.filename = pathname

            @templates[name] = template
        end

        template
    end

    def write_file(tpl, name, &block)
        file = File.open File.join(@destination, name), File::CREAT | File::TRUNC | File::WRONLY, 0644

        file.write get_template(tpl).result(binding)
    end

    def generate_enums(protocol)
        enums = protocol.groups.map do |group|
            group_name = group.id.to_s

            group_enum = Enum.new group_name
            group_enum.values = group.messages.map do |m|
                [
                    [ group_name, m.id.to_s ].join("_"),
                    m.type
                ]
            end
            group_enum.add_minmax!(group_name + "_")

            group_enum
        end

        cmd_enum = Enum.new "CMD"
        cmd_enum.values = protocol.groups.map { |g| [ "CMD_" + g.id.to_s, g.type ] }
        cmd_enum.add_minmax! "CMD_"
        enums << cmd_enum

        enums
    end
end