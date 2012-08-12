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

message_group :NET, 8 do |g|
    g.out :GET_PLMN_SELECTION, 2, :get
    g.in :GET_PLMN_SELECTION_REPLY, 2 do |m|
        m.u8 :selection
    end

    g.out :SET_PLMN_SELECTION, 2, :set do |m|
        m.u8 :selection, :type => :enum, :values => {
            Automatic: 2,
            Manual:    3
        }
        m.vector :network, :read_length => 6
        m.u8 :unknown1 # always 0xFF
    end

    g.out :GET_CURRENT_PLMN, 3, :get
    g.in :GET_CURRENT_PLMN_REPLY, 3 do |m|
        m.u8 :unknown1 # Must be 0x02
        m.u16 :unknown2
        m.vector :plmn, :read_length => 6
        m.u16 :unknown3
    end

    g.unsolicited :GET_CURRENT_PLMN_REPLY

    g.out :GET_PLMN_LIST, 4, :get
    g.in :GET_PLMN_LIST_REPLY, 4 do |m|
        m.u8 :count
        m.vector :data, :read_length => "stream.remainingBytes()"

        # for each PLMN: (10 bytes)
        # +0 u8 :status, :type => enum, :values = {
        #        Available: 2
        #        Current:   3,
        #        Forbidden: 4,
        #    }
        # +1 vector :plmn, :read_length => 6
        # +7 u8 :unknown2
        # +8 u8 :unknown3
        # +9 u8 :unknown4
    end

    g.out :GET_NETWORK_REGISTRATION, 5, :get do |m|
        m.u8 :unknown1 # Always 0xFF
        m.u8 :serviceDomain, :type => :enum, :values => {
            Voice: 0x02,
            Data:  0x03
        }
    end
    g.in :GET_NETWORK_REGISTRATION_REPLY, 5 do |m|
        m.u8  :act # Unclear
        m.u8  :serviceDomain, :type => :enum, :values => {
            Voice: 0x02,
            Data:  0x03
        }
        m.u8  :registrationStatus, :type => :enum, :values => {
            NotRegistered:                   1,
            HomeNetwork:                     2,
            SearchingEmergencyOnly:          3,
            RegistrationDeniedEmergencyOnly: 4,
            UnknownEmergencyOnly:            5,
            Roaming:                         6,
            ANDROID_STATE_6:                 7, # Not defined, but used by Samsung.
            GPRSNotAllowed:                  8
        }
        m.u8  :unknown1 # Not used by RIL
        m.u16 :lac
        m.u32 :cid
        m.u8  :unknown2 # Not used by RIL
    end
    g.unsolicited :GET_NETWORK_REGISTRATION_REPLY

    g.out :GET_BAND_SELECTION, 7, :get
    g.in :GET_BAND_SELECTION_REPLY, 7 do |m|
        m.u8  :unknown1 # Not used by the RIL
        m.u32 :band, :type => :enum, :values => {
            Automatic: 0x40000000,
            EURO:      0x16,
            US:        0x129,
            JPN:       0x210,
            AUS:       0x116,
            AUS2:      0x106
        }
    end

    g.out :SET_BAND_SELECTION, 7, :set do |m|
        m.u8  :unknown1 # Always 0x02
        m.u32 :band, :type => :enum, :values => {
            Automatic: 0x40000000,
            EURO:      0x16,
            US:        0x129,
            JPN:       0x210,
            AUS:       0x116,
            AUS2:      0x106
        }
    end

    g.out :GET_MODE_SELECT, 10, :get
    g.in :GET_MODE_SELECT_REPLY, 10 do |m|
        m.u8 :mode
    end

    g.out :SET_MODE_SELECT, 10, :set do |m|
        m.u8 :mode
    end
end
