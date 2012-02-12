--
-- Free RIL implementation for Samsung Android-based smartphones.
-- Copyright (C) 2012  Sergey Gridasov <grindars@gmail.com>
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.
--

do
    local p_phonet = Proto("phonet", "PHONET")
    local f_rdev   = ProtoField.uint8("phonet.rdev",    "Receiving device", base.HEX);
    local f_sdev   = ProtoField.uint8("phonet.sdev",    "Sending device",   base.HEX);
    local f_res    = ProtoField.uint8("phonet.res",     "Resource ID",      base.HEX);
    local f_length = ProtoField.uint16("phonet.length", "Frame length",     base.DEC);
    local f_robj   = ProtoField.uint8("phonet.robj",    "Receiving object", base.HEX);
    local f_sobj   = ProtoField.uint8("phonet.sobj",    "Sending object",   base.HEX);

    local phonet_table = DissectorTable.new("phonet.res", "PHONET resource type", FT_UINT8, BASE_HEX);

    p_phonet.fields = { f_rdev, f_sdev, f_res, f_length, f_robj, f_sobj }

    function p_phonet.dissector(buf, ws_pkt, root)

        local tree = root:add(p_phonet, buf(0, 7))
        tree:add(f_rdev,   buf(0, 1))
        tree:add(f_sdev,   buf(1, 1))
        tree:add(f_res,    buf(2, 1))
        tree:add(f_length, buf(3, 2), buf(3, 2):uint() + 6)
        tree:add(f_robj,   buf(5, 1))
        tree:add(f_sobj,   buf(6, 1))

        ws_pkt.cols.src = string.format("%02X:%02X", buf(1, 1):uint(), buf(6, 1):uint())
        ws_pkt.cols.dst = string.format("%02X:%02X", buf(0, 1):uint(), buf(5, 1):uint())
        ws_pkt.cols.protocol = "PHONET"

        local resource = buf(2, 1):uint()
        local data = buf(7):tvb()

        ws_pkt.cols.info = string.format("Frame with resource %02X", resource);
        phonet_table:try(resource, data, ws_pkt, root)
    end

    DissectorTable.get("sll.ltype"):add(245, p_phonet)
end