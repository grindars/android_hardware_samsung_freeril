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

message_group :SND, 9 do |g|
    g.out :SET_VOLUME_CTRL, 1, :set do |m|
        m.u8 :device
        m.u8 :volume
    end

    g.out :GET_MIC_MUTE, 2, :get
    g.in  :GET_MIC_MUTE_REPLY, 2 do |m|
        m.u8 :mute
    end

    g.out :SET_MIC_MUTE, 2, :set do |m|
        m.u8 :mute
    end

    g.out :SET_AUDIO_PATH, 3, :set do |m|
        m.u8 :path
    end

    g.out :SET_LOOPBACK_CTRL, 5, :set do |m|
        m.u8 :ctrl
    end

    g.out :SET_VOICE_RECORDING_CTRL, 6, :set do |m|
        m.u8 :record
    end

    g.out :SET_VIDEO_CALL_CTRL, 7, :set do |m|
        m.u8 :clock
    end

    g.out :EXEC_CLOCK_CTRL, 9, :exec do |m|
        m.u8 :clock
    end

    g.out :SET_TWO_MIC_CTRL, 11, :set do |m|
        m.u8 :param1
        m.u8 :param2
    end

    g.out :SET_DHA_CTRL, 12, :set do |m|
        m.u8 :mode
        m.u8 :select
        m.vector :parameter, :read_length => 24
    end

end
