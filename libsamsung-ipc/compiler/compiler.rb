#!/usr/bin/env ruby
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

require "erb"

require_relative "field.rb"
require_relative "structure.rb"
require_relative "message.rb"
require_relative "message_group.rb"
require_relative "protocol.rb"
require_relative "enum.rb"
require_relative "generator.rb"

if ARGV.count != 2
    $stderr.puts "Usage: compiler.rb <DESTINATION DIRECTORY> <SOURCE DIRECTORY>"

    exit 1
end

protocol = Protocol.new

Dir.foreach(ARGV[1]) do |filename|
    filename = File.join(ARGV[1], filename)

    if File.file? filename
        content = File.read filename

        protocol.evaluate content, filename
    end
end

generator = Generator.new ARGV[0]
generator.implement protocol