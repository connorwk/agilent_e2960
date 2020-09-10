-- Port 1029 GUI Comms

gui_proto = Proto("E2960-G", "Agilent E2960 port 1029 GUI Comms")

local to_rspc_types = {
    [0x80] = "ACK/RSP Requested",
    [0x00] = "Broadcast",
}
local from_rspc_types = {
    [0x80] = "ACK/RSP",
    [0x00] = "Broadcast",
}

-- a table of all of our Protocol's fields
local gui_fields =
{
	info_addr = ProtoField.bytes("e2960-g.addr1", "Address", base.SPACE),
	to_info_rspc = ProtoField.uint8("e2960-g.rspc", "RSPCode", base.HEX, to_rspc_types),
	from_info_rspc = ProtoField.uint8("e2960-g.rspc", "RSPCode", base.HEX, from_rspc_types),
	info_token = ProtoField.bytes("e2960-g.token", "Token", base.SPACE),
	info_unk1 = ProtoField.bytes("e2960-g.unk1", "Unknown 1", base.SPACE),
	info_data = ProtoField.bytes("e2960-g.data", "Data", base.SPACE),
	info_raw = ProtoField.bytes("e2960-g.raw", "GUI Data Raw", base.SPACE),
}
gui_proto.fields = gui_fields

function gui_proto.dissector(buffer, pinfo, tree)
    len = buffer:len()
    if len == 0 then return end

    pinfo.cols.protocol = "E2960-G"

    local subtree = tree:add(gui_proto, buffer(), "E2960 GUI Comms")
	
	subtree:add(gui_fields.info_addr, buffer(0, 4))
	
	-- To card
	if pinfo.dst_port == 1029 then
		subtree:add(gui_fields.to_info_rspc, buffer(4, 1))
	end
	
	-- From card
	if pinfo.src_port == 1029 then
		subtree:add(gui_fields.from_info_rspc, buffer(4, 1))
	end
	
	subtree:add(gui_fields.info_token, buffer(5, 3))
	subtree:add(gui_fields.info_unk1, buffer(8, 4))
	subtree:add(gui_fields.info_data, buffer(12))
	
	subtree:add(gui_fields.info_raw, buffer(0))
end

local tcp_port = DissectorTable.get("tcp.port")
tcp_port:add(1029, gui_proto)

-- UDP Broadcast

broadcast_proto = Proto("E2960-B", "Agilent E2960 port 7890 UDP broadcast")

-- a table of all of our Protocol's fields
local broadcast_fields =
{
	info_boot_seq = ProtoField.uint32("e2960-b.boot_seq", "NCP_BOOT_SEQUENCE", base.UINT32),
	info_sn = ProtoField.string("e2960-b.sn", "NCP_SERIAL_NUMBER", base.ASCII),
	info_raw = ProtoField.bytes("e2960-b.raw", "Broadcast Raw", base.SPACE),
}

broadcast_proto.fields = broadcast_fields

function broadcast_proto.dissector(buffer, pinfo, tree)
    len = buffer:len()
    if len == 0 then return end

    pinfo.cols.protocol = "E2960-B"

    local subtree = tree:add(broadcast_proto, buffer(), "E2960 UDP Broadcast")
	
    subtree:add(broadcast_fields.info_boot_seq, buffer(2, 4))
    subtree:add(broadcast_fields.info_sn, buffer(6, 13))
    subtree:add(broadcast_fields.info_raw, buffer(0))
end

local udp_port = DissectorTable.get("udp.port")
udp_port:add(7890, broadcast_proto)

-- Port 1027 Metadata

metadata_proto = Proto("Agilent-E2960", "Agilent E2960 port 1027 metadata")

-- a table of all of our Protocol's fields
local metadata_fields =
{
    info_key = ProtoField.string("e2960.key", "Metadata Key", base.ASCII),
    info_value = ProtoField.string("e2960.value", "Metadata Value", base.ASCII),
    info_raw = ProtoField.bytes("e2960.raw", "Metadata Raw", base.SPACE),
}

-- register the ProtoFields
metadata_proto.fields = metadata_fields

function metadata_proto.dissector(buffer, pinfo, tree)
    len = buffer:len()
    if len == 0 then return end

    pinfo.cols.protocol = "E2960"

    local subtree = tree:add(metadata_proto, buffer(), "E2960 Metadata")

    eq_off = string.find(buffer(0):raw(), "=", 0)
    if eq_off ~= nil then
        subtree:add(metadata_fields.info_key, buffer(0, eq_off - 1))
        subtree:add(metadata_fields.info_value, buffer(eq_off))
    end
    subtree:add(metadata_fields.info_raw, buffer(0))
end

local tcp_port = DissectorTable.get("tcp.port")
tcp_port:add(1027, metadata_proto)