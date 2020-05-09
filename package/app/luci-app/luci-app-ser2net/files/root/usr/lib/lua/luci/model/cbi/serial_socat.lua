--[[
LuCI - Lua Configuration Interface
Copyright 2010 Jo-Philipp Wich <xm@subsignal.org>
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
--]]


require("luci.sys")

--local st = require "luci.tools.firewall"

local map, section, net = ...

-- /etc/config/serial_socat
m = Map("serial_socat", translate("Serial<->Networking"), translate("Serial data and network data exchanged."))


--
-- Forwards Table
--

--[[
s = m:section(TypedSection, "serial_socat", translate("Serial <-> Network Forward Table"))
s.template  = "cbi/tblsection"
s.addremove = true
s.anonymous = true
s.sortable  = true
--]]

--s.extedit   = ds.build_url("admin/services/socat/tcp-udp-forwards/%s")
--s.template_addremove = "socat/cbi_add_tcp_udp_forward"

--opt_forward_name
function opt_forward_name(s, t, ...)
	local o = s:option(t, "forward_name", ...)

	function o.cfgvalue(self, section)
		return self.map:get(section, "forward_name") or
			self.map:get(section, "_forward_name") or "-"
	end

	function o.write(self, section, value)
		if value ~= "-" then
			self.map:set(section, "forward_name", value)
			self.map:del(section, "_forward_name")
		else
			self:remove(section)
		end
	end

	function o.remove(self, section)
		self.map:del(section, "forward_name")
		self.map:del(section, "_forward_name")
	end

	return o
end



--opt_forward_name(s, DummyValue, translate("Forward Name"))


--[[
serial = s:option(DummyValue, "match", translate("Serial Arguments"))
serial.rawhtml = true
serial.width   = "50%"
function serial.cfgvalue(self, s)
--	return "<small>%s<br />%s<br />%s</small>" % {
	return "<small><br />%s<br /></small>" % {
		forward_serialh_txt(self, s)
	}
end


dest = s:option(DummyValue,translate("Network Arguments"))
dest.rawhtml = true
dest.width   = "40%"
function dest.cfgvalue(self, s)
--	local z = st.fmt_zone(self.map:get(s, "dest"), translate("any zone"))
	local a = st.fmt_ip(self.map:get(s, "server_addr"))
	local p = st.fmt_port(self.map:get(s, "server_port"))
	local ptl = st.fmt_port(self.map:get(s, "protocol"))

	if ptl == "udp-connect" or ptl == "tcp-connect" then
		return translatef("IP:%s Port:%s",a,p)
	else 
		return translatef("Listen:%s",p)
	end
	
end

st.opt_enabled(s, Flag, translate("Enable")).width = "1%"
--]]

s = m:section(TypedSection, "serial_socat", "")

--Don't Allow the user to create and remove the interfaces
s.addremove = false
s.anonymous = true

s:tab("basic", translate("Basic Configuration"))

enabled = s:taboption("basic", Flag, "enabled", translate("Enable"))
enabled.default = 0

--enabled = s:taboption("basic", ListValue, "enabled", translate("Enable"))                   
--enabled:value("0", "OFF")
--enabled:value("1", "ON")                                                                           
--enabled.default = "0"  




log_enabled = s:taboption("basic", Flag, "socat_log_enable", translate("Log Enable"))
log_enabled.default = 0

--the thrid par is the config par
f = s:taboption("basic", Value, "forward_name", translate("Forward Name"))
f.rmempty = false
f.default = "RS232<->ETH"
--DeviceId:depends("enabled", "1")


--interval = s:taboption("basic", Value, "interval", translate("Heartbeat Interval (s)"))
--interval.rmempty = false
--interval.default = "60"
--interval:depends("enabled", "1")

--heartbeat = s:taboption("basic", Value, "heartbeat", translate("Heartbeat Content"))
--heartbeat.rmempty = false
--heartbeat.default = "FE"
--heartbeat:depends("enabled", "1")




DataWaitTime = s:taboption("basic", Value, "MaxWaitTime", translate("Data Max Wait Time(S)"))
DataWaitTime.rmempty = false
DataWaitTime.default = "30"
--DataWaitTime:depends("enabled", "1")


--ReDialTime = s:taboption("basic", Value, "ReDialTime", translate("ReDialTime"))
--ReDialTime.rmempty = false
--ReDialTime.default = "15"
--ReDialTime:depends("enabled", "1")


s:tab("serial", translate("Serial Configuration"))


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


serial = s:taboption("serial", Value, "serial", translate("Serial device"))
serial.rmempty = false
get_device_name(serial)

baudrate = s:taboption("serial", ListValue, "baudrate", translate("Baudrate (bps)"))
baudrate:value("B300", "300")
baudrate:value("B1200", "1200")
baudrate:value("B2400", "2400")
baudrate:value("B4800", "4800")
baudrate:value("B9600", "9600")
baudrate:value("B19200", "19200")
baudrate:value("B38400", "38400")
baudrate:value("B57600", "57600")
baudrate:value("B115200", "115200")
baudrate.default = "B9600"

databit = s:taboption("serial", ListValue, "databit", translate("Databit"))
databit:value("CS5", "5")
databit:value("CS6", "6")
databit:value("CS7", "7")
databit:value("CS8", "8")
databit.default = "CS8"

parity_check = s:taboption("serial", ListValue, "parity_check", translate("Odd-Even Check"))
parity_check:value("NONE", translate("None Check"))
parity_check:value("PARODD", translate("Odd Check"))
parity_check:value("PARENB", translate("Even Check"))
parity_check.default = "NONE"

stopbit = s:taboption("serial", ListValue, "stopbit", translate("Stopbit"))
stopbit:value("", "1")
stopbit:value("CSTOPB", "2")
stopbit.default = "0"


s:tab("network", translate("Networking Configuration"))

o = s:taboption("network", ListValue, "protocol", translate("Net Mode"))
o:value("tcp-connect", "TCP-CLIENT")
o:value("tcp-listen", "TCP-SERVER")
o:value("udp-connect", "UDP-CLIENT")
o:value("udp-listen", "UDP-SERVER")
o.default = "tcp-connect"


o = s:taboption("network",Value,"server_addr",translate("Target IP address"))
o.rmempty = true
--o.datatype = "neg(ip4addr)"
o.placeholder = translate("any")
o:depends("protocol", "tcp-connect")       
o:depends("protocol", "udp-connect")       

luci.sys.net.ipv4_hints(function(ip, name)
	o:value(ip, "%s (%s)" %{ ip, name })
end)



o = s:taboption("network", Value, "server_port", translate("Server Port"))
o.datatype = "neg(portrange)"





reconnect = s:taboption("network", Value, "ReConnectTime", translate("Re Connect Time(S)"))   
reconnect:depends("protocol", "tcp-connect")       
reconnect:depends("protocol", "udp-connect")                                                        
reconnect.datatype="neg(uinteger)"
reconnect.default="30"







local function forward_serialh_txt(self, s)
	return "Serial device"..":%s ".."Baudrate"..":%s ".."Check"..":%s" %{
		self.map:get(s, "serial"),
	    self.map:get(s, "baudrate"),
		self.map:get(s, "parity_check")
	}
end


function opt_network_arg(s, t, ...)
	local o = s:option(t, "target_name", ...)

	function o.cfgvalue(self, section)
		return self.map:get(section, "target_name") or
			self.map:get(section, "_target_name") or "-"
	end

	function o.write(self, section, value)
		if value ~= "-" then
			self.map:set(section, "target_name", value)
			self.map:del(section, "_target_name")
		else
			self:remove(section)
		end
	end

	function o.remove(self, section)
		self.map:del(section, "target_name")
		self.map:del(section, "_target_name")
	end

	return o
end



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


--[[
local apply = luci.http.formvalue("cbi.apply")
if apply then
	io.popen("/etc/init.d/socat_ser2net restart")
end
--]]


return m
