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
    local p_boot = Proto("hsic6260_boot", "6260 HSIC Bootloader")

    local f_checksum       = ProtoField.uint16("hsic6260_boot.checksum",    "Checksum", base.HEX)
    local f_type           = ProtoField.uint16("hsic6260_boot.type",        "Type", base.HEX)
    local f_data_length    = ProtoField.uint16("hsic6260_boot.data_length", "Data length", base.DEC)

    local boot_table = DissectorTable.new("hsic6260_boot.type", "6260 HSIC Bootloader packet type", FT_UINT8, BASE_HEX)

    p_boot.fields = { f_checksum, f_type, f_data_length }

    function p_boot.dissector(buf, ws_pkt, root)
        local tree = root:add(p_boot, buf(0, math.min(8, buf:len())))

        ws_pkt.cols.protocol = "Bootloader"

        if buf:len() < 8 then
            ws_pkt.cols.info = "Short frame"
            tree:add("Short frame", buf)

            return
        end

        local checksum = buf(0, 2):le_uint()
        local type     = buf(2, 2):le_uint()
        local datalen  = buf(4, 4):le_uint()

        tree:add_le(f_checksum,    buf(0, 2))
        tree:add_le(f_type,        buf(2, 2), type)
        tree:add_le(f_data_length, buf(4, 4), datalen)

        if datalen + 8 <= buf:len() then
            ws_pkt.cols.info = string.format("Frame of type %04X", type)

            boot_table:try(type, buf(8, datalen):tvb(), ws_pkt, root)

            if datalen + 8 < buf:len() then
                root:add(p_boot, buf(datalen + 8), "Trailer")
            end
        else
            ws_pkt.cols.info = "Short frame"
        end
    end


    DissectorTable.get("phonet.res"):add(0, p_boot)
end

