-- Copyright 2019 LEGEND Wich <466005858@QQ.COM>
-- Licensed to the public under the Apache License 2.0.

local sys = require "luci.sys"
local dsp = require "luci.dispatcher"
local st  = require "luci.tools.socat"
local m, s, o
arg[1] = arg[1] or ""
m = Map("net_socat",
	translate("TCP/UDP - Port Forwards"),
	translate("This page allows you to change advanced properties of the port \
	           forwarding entry. In most cases there is no need to modify \
			   those settings."))

return m





