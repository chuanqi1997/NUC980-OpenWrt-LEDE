--[[
LuCI - Lua Configuration Interface
Copyright 2010 Jo-Philipp Wich <xm@subsignal.org>
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
--]]

local ds = require "luci.dispatcher"
require("luci.sys")

--local st = require "luci.tools.firewall"

local map, section, net = ...


function get_device_name(s, ...)
	--
	local device_suggestions = nixio.fs.glob("/dev/tty[A-Z]*")
		or nixio.fs.glob("/dev/tts/*")
	if device_suggestions then
		local node
		for node in device_suggestions do
			s:value(node)
		end
	end
	--
end


-- /etc/config/ztk_service
m = Map("ztk_service", translate("ZTK Device<->MQTT Broker"), translate("ZTK ATTLOG to MQTT Message."))

s = m:section(TypedSection, "ztk_service", "")
--Don't Allow the user to create and remove the interfaces
s.addremove = false
s.anonymous = true


s:tab("server", translate("Server Configuration"))

e = s:taboption("server", Flag, "enabled", translate("Enable"))
e.default = 0

p = s:taboption("server", Value, "HTTP_PORT", translate("HTTP PORT"))
p.datatype = "neg(portrange)"
p.rmempty = false
p.default = "8080"


serial = s:taboption("server", Value, "DOOR_UART", translate("Door Monitor device"))
serial.rmempty = false
get_device_name(serial)
serial.default="/dev/ttyS1"

--[[
log = s:taboption("server", Value, "log", translate("log"))
log.rmempty = false
log.datatype="loraeui"
log.template = "cbi/run_status"
--]]


--s = m:section(TypedSection, "ztk_service", translate(""))
--s.template  = "ztk_service/cbl_show_logn"
--s.addremove = false
--s.anonymous = true
--s.sortable  = true
--admin/services/ptl-relay/collect
--s.extedit   = ds.build_url("admin/services/ztk_service/%s")
--s.template_addremove = "ztk_service/cbl_show_log"




s:tab("mqtt client", translate("MQTT Configuration"))

o = s:taboption("mqtt client",Value,"MQTT_HOST",translate("MQTT Broker IP address"))
o.rmempty = true
o.datatype = "neg(ip4addr)"
o.default = "192.168.1.233"
o.placeholder = translate("any")



o = s:taboption("mqtt client", Value, "MQTT_PORT", translate("MQTT Broker Port"))
o.datatype = "neg(portrange)"
o.default = "1883"


t = s:taboption("mqtt client", Value, "MQTT_TOPIC", translate("MQTT Publish Topic"))                   
--reconnect.datatype="neg(uinteger)"
t.default="/topic/ATTLOG"






function opt_enabled(s, t, ...)
	if t == luci.cbi.Button then
		local o = s:option(t, "__enabled")
		function o.render(self, section)
			if self.map:get(section, "enabled") ~= "0" then
				self.title      = tr("Rule is enabled")
				self.inputtitle = tr("Disable")
				self.inputstyle = "reset"
			else
				self.title      = tr("Rule is disabled")
				self.inputtitle = tr("Enable")
				self.inputstyle = "apply"
			end
			t.render(self, section)
		end
		function o.write(self, section, value)
			if self.map:get(section, "enabled") ~= "0" then
				self.map:set(section, "enabled", "0")
			else
				self.map:del(section, "enabled")
			end
		end
		return o
	else
		local o = s:option(t, "enabled", ...)
		o.default = "1"
		return o
	end
end


return m
