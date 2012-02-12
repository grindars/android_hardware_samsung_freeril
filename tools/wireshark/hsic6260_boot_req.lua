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
    local p_boot_force_reset = Proto("hsic6260_force_reset", "ReqForceHwReset")
    local f_magic = ProtoField.uint32("hsic6260_force_reset.magic", "Magic number", base.HEX)

    p_boot_force_reset.fields = { f_magic }

    function p_boot_force_reset.dissector(buf, ws_pkt, root)
        local tree = root:add(p_boot_force_reset, buf(0, 4))
        ws_pkt.cols.info = "ReqForceHwReset"

        tree:add_le(f_magic, buf(0, 4))
    end

    DissectorTable.get("hsic6260_boot.type"):add(0x0208, p_boot_force_reset)
end

do
    local p_boot_set_address = Proto("hsic6260_set_address", "ReqFlashSetAddress")
    local f_address = ProtoField.uint32("hsic6260_set_address.address", "Address", base.HEX)

    p_boot_set_address.fields = { f_address }

    function p_boot_set_address.dissector(buf, ws_pkt, root)
        local tree = root:add(p_boot_set_address, buf(0, 4))
        ws_pkt.cols.info = "ReqFlashSetAddress"

        tree:add_le(f_address, buf(0, 4))
    end

    DissectorTable.get("hsic6260_boot.type"):add(0x0802, p_boot_set_address)
end

do
    local p_boot_write_block = Proto("hsic6260_write_block", "ReqFlashWriteBlock")
    local data_dis = Dissector.get("data")

    function p_boot_write_block.dissector(buf, ws_pkt, root)
        local tree = root:add(p_boot_write_block, buf())
        ws_pkt.cols.info = "ReqFlashWriteBlock"

        data_dis:call(buf, ws_pkt, root);
    end

    DissectorTable.get("hsic6260_boot.type"):add(0x0804, p_boot_write_block)
end
