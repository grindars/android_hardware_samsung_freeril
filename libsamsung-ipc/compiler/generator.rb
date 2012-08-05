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

class Generator
    TYPE_MAP = {
        u8:     'uint8_t',
        u16:    'uint16_t',
        u32:    'uint32_t',
        vector: 'std::vector<unsigned char>',
        data: 'raw'
    }

    def initialize(destination)
        @destination = destination
    end

    def implement(protocol)
        enums = extract_enums(protocol)
        messages = extract_messages(enums, protocol)

        write_header "MessageTypes.h",         generate_enums(enums)
        write_header "MessageFactory.h",       generate_factory_header(enums)
        write_source "MessageFactory.cpp",     generate_factory(enums)
        write_header "Messages.h",             generate_messages_header(messages)
        write_source "Messages.cpp",           generate_messages(messages)
        write_header "IUnsolicitedReceiver.h", generate_unsolicited_receiver(messages)
    end

    protected

    def write_source(name, content)
        file = File.open File.join(@destination, name), File::CREAT | File::TRUNC | File::WRONLY, 0644

        file.puts "/*"
        file.puts " * Generated by the protocol compiler. Do not edit."
        file.puts " */"
        file.puts ""

        file.write content

        file.puts ""
    end

    def write_header(name, content)
        file = File.open File.join(@destination, name), File::CREAT | File::TRUNC | File::WRONLY, 0644

        file.puts "/*"
        file.puts " * Generated by the protocol compiler. Do not edit."
        file.puts " */"
        file.puts ""

        tag = "__#{name.upcase.sub ".", "__"}__"
        file.puts "#ifndef #{tag}"
        file.puts "#define #{tag}"
        file.puts ""

        file.write content

        file.puts ""
        file.puts "#endif"
    end

    def extract_enums(protocol)
        enums = {
            :CMD => {}
        }

        protocol.groups.each do |group|
            enums[:CMD][group.id] = { :type => group.type, :direction => :in, :op => :exec }

            enums[group.id] ||= {}

            group.messages.each do |msg|
                enums[group.id][msg.id] = {
                    type: msg.type,
                    direction: msg.direction,
                    fields: msg.fields,
                    op: msg.op
                }
            end
        end

        enums.each do |id, fields|
            enums[id][:FIRST], enums[id][:LAST] = fields.minmax { |a, b| a[1][:type] <=> b[1][:type] }.map { |item| "#{id.to_s}_#{item[0].to_s}".intern }
        end
        enums
    end

    def extract_messages(enums, protocol)
        enums.reject { |k, v| k == :CMD }
             .map { |k, v|
                    group = protocol.groups.find { |g| g.id == k }

                    v.reject { |fk, fv| fv.kind_of? Symbol }
                     .map { |fk, fv|
                            {
                                :id          => class_name_for("#{k.to_s}_#{fk.to_s}"),
                                :command     => enums[:CMD][k][:type],
                                :unsolicited => group.unsolicited_messages.include?(fk),
                                :op          => enums[:CMD][k][:op]
                            }.merge fv
                          }
                  }
             .flatten
    end

    def generate_enums(enums)

        definitions = enums.map do |id, fields|
            mapped_fields = fields.map do |field_id, value|
                element_id = [ id, field_id ].map(&:to_s).join("_")

                value = value[:type] unless value.kind_of? Symbol

                "            #{element_id} = #{value.to_s}"
            end

            "        enum #{id} {\n#{mapped_fields.join ",\n"}\n        };\n"
        end.join

        "namespace SamsungIPC {\n" \
        "    namespace Messages {\n" \
        "#{definitions}" \
        "    }\n" \
        "}\n"
    end

    def generate_factory_header(enums)
        factoryMethods = []
        dispatchTables = []

        enums.each do |id, fields|
            name = id.to_s

            factoryMethods << "            static Message *create#{name}(uint8_t command, uint8_t subcommand);"
            dispatchTables << "            static const MessageFactoryFunction m_dispatch#{name}[#{name}_LAST - #{name}_FIRST + 1];"
        end
        <<-eos
#include "Message.h"

namespace SamsungIPC {
    namespace Messages {

        class Factory {
        public:

            inline static Message *create(uint8_t command, uint8_t subcommand) {
                return createCMD(command, subcommand);
            }

        private:
            typedef Message *(*MessageFactoryFunction)(uint8_t command, uint8_t subcommand);

            static Message *createFromDispatchTable(const MessageFactoryFunction table[],
                                                    int first, int last, int type,
                                                    uint8_t command, uint8_t subcommand);

#{factoryMethods.join("\n")}

#{dispatchTables.join("\n")}

        };
    }
}
eos
    end

    def generate_factory(enums)
        factoryMethods = []
        dispatchTables = []

        enums.each do |id, fields|
            name = id.to_s

            factoryMethods << <<-eos
Message *Factory::create#{name}(uint8_t command, uint8_t subcommand) {
    return createFromDispatchTable(Factory::m_dispatch#{name}, (int) #{name}_FIRST, (int) #{name}_LAST, #{id == :CMD ? "command" : "subcommand"},
                                   command, subcommand);
}
eos
            types = fields.reject { |k, v| v.kind_of? Symbol }
            min, max = types.minmax { |a, b| a[1][:type] <=> b[1][:type] }.map { |v| v[1][:type] }
            mapped_fields = [ "    NULL" ] * (max - min + 1)

            types.each do |k, v|
                if id == :CMD
                    mapped_fields[v[:type] - min] = "    create#{k.to_s}"
                elsif v[:direction] == :in
                    msg_name = "#{name}_#{k.to_s}"

                    mapped_fields[v[:type] - min] = "    create#{msg_name}"

                    factory = "new #{class_name_for msg_name}()"
                    factoryMethods << <<-eos
static Message *create#{msg_name}(uint8_t command, uint8_t subcommand) {
    (void) command;
    (void) subcommand;

    return #{factory};
}
eos
                end
            end

            dispatchTables << <<-eos
const Messages::Factory::MessageFactoryFunction Messages::Factory::m_dispatch#{name}[#{name}_LAST - #{name}_FIRST + 1] = {
#{mapped_fields.join(",\n")}
};
eos
        end

    <<-eos
#include "MessageFactory.h"
#include "Messages.h"

using namespace SamsungIPC;
using namespace Messages;

Message *Factory::createFromDispatchTable(const MessageFactoryFunction table[],
                                          int first, int last, int type,
                                          uint8_t command, uint8_t subcommand) {
    if(type < first || type > last)
        return NULL;

    MessageFactoryFunction factory = table[type - first];

    if(factory)
        return factory(command, subcommand);
    else
        return NULL;
}

#{factoryMethods.join("\n")}
#{dispatchTables.join("\n")}
eos
    end

    def generate_messages_header(messages)
        classes = messages.map do |msg|
            field_enums = []
            accessors = []
            data_fields = []

            msg[:fields].each do |field|
                storage_type = TYPE_MAP[field.type]

                if field.type == :data
                    data_fields << "            uint8_t m_#{field.name}[#{field.parameters[:size]}];"

                elsif field.parameters[:type] == :enum
                    values = field.parameters[:values].map { |key, value| "                #{key.to_s} = #{value.to_s}" }
                    enum_type = field.name.to_s.capitalize
                    field_enums << <<-eos
            enum #{enum_type} {
#{values.join(",\n")}
            };
eos
                    accessors << <<-eos
            inline #{enum_type} #{field.name}() const { return (#{enum_type}) m_#{field.name}; }
            inline void set#{field.name.capitalize}(#{enum_type} #{field.name}) { m_#{field.name} = (#{storage_type}) #{field.name}; }
eos

                    data_fields << "            #{storage_type} m_#{field.name};"
                elsif field.type == :vector
                    accessors << <<-eos
            inline const #{storage_type} &#{field.name}() const { return m_#{field.name}; }
            inline void set#{field.name.capitalize}(const #{storage_type} &#{field.name}) { m_#{field.name} = #{field.name}; }
eos
                    data_fields << "            #{storage_type} m_#{field.name};"
                else
                    accessors << <<-eos
            inline #{storage_type} #{field.name}() const { return m_#{field.name}; }
            inline void set#{field.name.capitalize}(#{storage_type} #{field.name}) { m_#{field.name} = #{field.name}; }
eos
                    data_fields << "            #{storage_type} m_#{field.name};"
                end


            end

            <<-eos
        class #{msg[:id]}: public Message {
        public:
            virtual uint8_t command() const;
            virtual uint8_t subcommand() const;
            virtual RequestType requestType() const;
            virtual std::string inspect() const;
            virtual bool deliver(IUnsolicitedReceiver *receiver);

            static inline bool isTypeOf(Message *message) {
                return message->command() == #{msg[:command]} && message->subcommand() == #{msg[:type]};
            }

#{field_enums.join("\n")}
#{accessors.join("\n")}
            virtual bool readFromStream(DataStream &stream);
            virtual bool writeToStream(DataStream &stream);

        private:
#{data_fields.join("\n")}
        };
eos
        end

        <<-eos
#include <string.h>
#include <vector>

#include "Message.h"

namespace SamsungIPC {
    namespace Messages {
#{classes.join("\n")}
    }
}
eos
    end

    def generate_messages(messages)
        methods = []

        messages.each do |message|
            field_inspectors = []
            field_readers = []
            field_writers = []

            message[:fields].each do |field|
                case field.type
                when :u8
                    cast = "(unsigned int) "
                else
                    cast = ""
                end

                case field.type
                when :data
                    field_inspectors << "    stream << \"  #{field.name} = <raw data>\\n\";"
                    field_readers << "        stream.readRawData(m_#{field.name}, sizeof(m_#{field.name}));"
                    field_writers << "        stream.writeRawData(m_#{field.name}, sizeof(m_#{field.name}));"

                when :vector
                    field_inspectors << "    stream << \"  #{field.name} = <vector of size \" << m_#{field.name}.size() << \">\\n\";"
                    field_readers << "        m_#{field.name}.resize(#{field.parameters[:read_length]});"
                    field_readers << "        stream.readRawData(&m_#{field.name}[0], m_#{field.name}.size());"
                    field_writers << "        stream.writeRawData(&m_#{field.name}[0], m_#{field.name}.size());"

                else
                    field_inspectors << "    stream << \"  #{field.name} = \" << #{cast}m_#{field.name} << \"\\n\";"
                    field_readers << "        stream.read(m_#{field.name});"

                    if field.parameters[:initialize]
                        field_writers << "        m_#{field.name} = #{field.parameters[:initialize]};"
                    end

                    field_writers << "        stream.write(m_#{field.name});"
                end
            end

            if message[:unsolicited]
                methods << <<-eos
bool #{message[:id]}::deliver(IUnsolicitedReceiver *receiver) {
    receiver->handle(this);

    return true;
}
eos
            else
                methods << <<-eos
bool #{message[:id]}::deliver(IUnsolicitedReceiver *receiver) {
    (void) receiver;

    return false;
}
eos
            end

            requestType =
                case message[:op]
                when :exec
                    "IPC_CMD_EXEC"

                when :get
                    "IPC_CMD_GET"

                when :cfrm
                    "IPC_CMD_CFRM"

                when :event
                    "IPC_CMD_EVENT"
                end

            methods << <<-eos
uint8_t #{message[:id]}::command() const {
    return #{message[:command]};
}

uint8_t #{message[:id]}::subcommand() const {
    return #{message[:type]};
}

Message::RequestType #{message[:id]}::requestType() const {
    return #{requestType};
}

std::string #{message[:id]}::inspect() const {
    std::stringstream stream;

    stream << "#{message[:id]} {\\n\\n";
#{field_inspectors.join("\n")}
    stream << "}\\n";

    return stream.str();
}

bool #{message[:id]}::readFromStream(DataStream &stream) {
    if(stream.caughtExceptionHere())
        return false;
    else {
#{field_readers.join("\n")}
        return true;
    }
}

bool #{message[:id]}::writeToStream(DataStream &stream) {
    if(stream.caughtExceptionHere())
        return false;
    else {
#{field_writers.join("\n")}
        return true;
    }
}
eos
        end

        <<-eos
#include <sstream>

#include "Messages.h"
#include "IUnsolicitedReceiver.h"
#include "DataStream.h"

using namespace SamsungIPC;
using namespace Messages;

#{methods.join("\n")}
eos
    end

    def generate_unsolicited_receiver(messages)
        handlers = []
        declarations = []

        messages.select { |m| m[:unsolicited] }.each do |m|
            handlers << "        virtual void handle(Messages::#{m[:id]} *message) = 0;"
            declarations << "        class #{m[:id]};"
        end

        <<-eos
namespace SamsungIPC {
    namespace Messages {
#{declarations.join("\n")}
    }

    class IUnsolicitedReceiver {
    public:
        virtual ~IUnsolicitedReceiver() {}

#{handlers.join("\n")}
    };
}
        eos
    end

    def class_name_for(message_name)
        return message_name.split("_").map(&:capitalize).join
    end

end