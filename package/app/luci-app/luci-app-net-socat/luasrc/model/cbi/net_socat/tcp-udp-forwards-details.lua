-- Copyright 2019 LEGEND Wich <466005858@QQ.COM>
-- Licensed to the public under the Apache License 2.0.

local sys = require "luci.sys"
local dsp = require "luci.dispatcher"
--local st  = require "luci.tools.firewall"
local st  = require "luci.tools.socat"

local m, s, o


function opt_source_name(s, t, ...)
	local o = s:option(t, "source_name", ...)

	function o.cfgvalue(self, section)
		return self.map:get(section, "source_name") or
			self.map:get(section, "_source_name") or "-"
	end

	function o.write(self, section, value)
		if value ~= "-" then
			self.map:set(section, "source_name", value)
			self.map:del(section, "_source_name")
		else
			self:remove(section)
		end
	end

	function o.remove(self, section)
		self.map:del(section, "source_name")
		self.map:del(section, "_source_name")
	end

	return o
end


function opt_target_name(s, t, ...)
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



arg[1] = arg[1] or ""

m = Map("net_socat",
	translate("TCP/UDP - Port Forwards"),
	translate("This page allows you to change advanced properties of the port \
	           forwarding entry. In most cases there is no need to modify \
			   those settings."))
--[[
m.redirect = dsp.build_url("admin/services/socat/tcp-udp-forwards")

if m.uci:get("socat", arg[1]) ~= "redirect" then
	luci.http.redirect(m.redirect)
	return
else
	local name = m:get(arg[1], "name") or m:get(arg[1], "_name")
	if not name or #name == 0 then
		name = translate("(Unnamed Entry)")
	end
	m.title = "%s - %s" %{ translate("TCP/UDP - Port Forwards"), name }
end


s = m:section(NamedSection, arg[1], "redirect", "")
s.anonymous = true
s.addremove = false

st.opt_enabled(s, Button)

--st.opt_name(s, Value, translate("Name"))


s = m:section(TypedSection, "redirect", translate("TCP/UDP Port Forwards Table"))
s.extedit   = dsp.build_url("admin/services/socat/tcp-udp-forwards")
s.template = "socat/cbi_show_tcp_udp_forward"

function s.create(self, section)
	local sn = m:formvalue("_newfwd.source_name")
	local tn = m:formvalue("_newfwd.target_name")
	local sz = m:formvalue("_newfwd.source_zone")
	local sp = m:formvalue("_newfwd.source_proto")
	local sa = m:formvalue("_newfwd.source_addr")
	local st = m:formvalue("_newfwd.source_port")

	local tz = m:formvalue("_newfwd.target_zone")
	local tp = m:formvalue("_newfwd.target_proto")
	local ta = m:formvalue("_newfwd.target_addr")
	local tt = m:formvalue("_newfwd.target_port")
	if sp == "other" or (sp and sa) then
		created = TypedSection.create(self, section)

		self.map:set(created, "name", sn.." To "..tn)

		self.map:set(created, "source_name",      sn)
		self.map:set(created, "target_name",      tn)
		self.map:set(created, "target",    "DNAT")
		self.map:set(created, "src",       tz or "wan")
		self.map:set(created, "dest",      sz or "lan")

		self.map:set(created, "source_proto",sp)
		self.map:set(created, "source_port", st)
		self.map:set(created, "source_addr", sa)

		self.map:set(created, "target_proto",tp)
		self.map:set(created, "target_addr", ta)
		self.map:set(created, "target_port", tt)	
	end

	if p ~= "other" then
		created = nil
	end
end

function s.parse(self, ...)
	TypedSection.parse(self, ...)
	if created then
		m.uci:save("socat")
		luci.http.redirect(dsp.build_url(
			"admin/services/socat/redirect", created
		))
	end
end

--]]



m.redirect = dsp.build_url("admin/services/net_socat/tcp-udp-forwards")

if m.uci:get("net_socat", arg[1]) ~= "net_socat" then
	luci.http.redirect(m.redirect)
	return
else
	local name = m:get(arg[1], "source_name").." <-> "..m:get(arg[1], "target_name")
	if not name or #name == 0 then
		name = translate("(Unnamed Entry)")
	end
	m.title = "%s - %s" %{ translate("Firewall - Port Forwards"), name }
end

s = m:section(NamedSection, arg[1], "net_socat", "")
s.anonymous = true
s.addremove = false

st.opt_enabled(s, Button)
--st.opt_source_name(s, Value, translate("Source Name"))

opt_source_name(s, Value, translate("Source Name"))

--sn = s:option(Value, "source_name", translate("Source Name"))
--sn.rmempty = false
--sn.default = "12345678"


--[[
o = s:option(Value, "src", translate("Source zone"))
o.nocreate = true
o.default = "wan"
o.template = "cbi/firewall_zonelist"

luci.sys.net.mac_hints(function(mac, name)
	o:value(mac, "%s (%s)" %{ mac, name })
end)
--]]



o = s:option(Value, "source_proto", translate("Source Protocol"))
o:value("tcp-connect", "TCP-CLIENT")
o:value("tcp-listen", "TCP-SERVER")
o:value("udp-connect", "UDP-CLIENT")
o:value("udp-listen", "UDP-SERVER")

o = s:option(Value, "source_addr",
	translate("Source IP address"),
	translate("Only match incoming traffic from this IP or range."))
o.rmempty = true
--o.datatype = "neg(ip4addr)"
o.placeholder = translate("any")

luci.sys.net.ipv4_hints(function(ip, name)
	o:value(ip, "%s (%s)" %{ ip, name })
end)


o = s:option(Value, "source_port",
	translate("Source port"),
	translate("Only match incoming traffic originating from the given source port or port range on the client host"))
o.rmempty = true
o.datatype = "neg(portrange)"
o.placeholder = translate("any")


o = s:option(Value, "source_rectime",
	translate("Reconnect Time (S)"))
o.rmempty = true
o.datatype = "neg(uinteger)"
o.placeholder = translate("any")
o:depends("source_proto", "tcp-connect")
o:depends("source_proto", "udp-connect")



--st.opt_target_name(s, Value, translate("Target Name"))
opt_target_name(s, Value, translate("Target Name"))


--[[
o = s:option(Value, "dest", translate("Target Zone"))
o.nocreate = true
o.default = "lan"
o.template = "cbi/firewall_zonelist"

--]]



o = s:option(Value, "target_proto", translate("Target Protocol"))
o:value("tcp-connect", "TCP-CLIENT")
o:value("tcp-listen", "TCP-SERVER")
o:value("udp-connect", "UDP-CLIENT")
o:value("udp-listen", "UDP-SERVER")


o = s:option(Value, "target_addr",
	translate("Target IP address"),
	translate("Only match incoming traffic directed at the given IP address."))

luci.sys.net.ipv4_hints(function(ip, name)
	o:value(ip, "%s (%s)" %{ ip, name })
end)

o.rmempty = true
--o.datatype = "neg(ip4addr)"
o.placeholder = translate("any")



o = s:option(Value, "target_port", translate("Target port"),
	translate("Match incoming traffic directed at the given " ..
		"destination port or port range on this host"))
o.datatype = "neg(portrange)"


o = s:option(Value, "target_rectime",
	translate("Reconnect Time (S)"))
o.rmempty = true
o.datatype = "neg(uinteger)"
o.placeholder = translate("any")
o:depends("target_proto", "tcp-connect")
o:depends("target_proto", "udp-connect")



--[[
local apply = luci.http.formvalue("cbi.apply")
if apply then
	io.popen("/etc/init.d/net_socat restart")
end
--]]




return m





