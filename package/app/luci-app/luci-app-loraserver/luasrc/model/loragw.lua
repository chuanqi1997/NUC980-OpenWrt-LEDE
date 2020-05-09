local table = require "table"
local string = require "string"
local nxo = require "nixio"
local sys = require "luci.sys"
local utl = require "luci.util"
local nfs = require "nixio.fs"

module "luci.model.loragw"

local sock

function init()
	sock = nxo.socket("unix","stream")
	if sock:connect("/tmp/run/lora_pkt_fwd.sock") then
		return _M
	else
		return nil
	end
end

function get(self)
	local t = {}
	while true do
		local buf = sock:read(4096)
		if buf then
			table.insert(t,buf)
			if string.len(buf) < 4096 then
				break
			end
		else
			break
		end
	end
	return table.concat(t)
end

function set(self, data)
	return sock:send(data)
end

function Close(self)
	return sock:close()
end